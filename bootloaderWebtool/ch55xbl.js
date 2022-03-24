var device = null;
var statusDiv = null;
var interfaceNumber = null; // Will be overwritten in open process
var endpointIn = null;
var endpointOut = null;

var bootloaderDetectCmd = [
	0xA1, 0x12, 0x00, 0x00, 0x11, 0x4D, 0x43, 0x55, 0x20, 0x49, 0x53, 0x50, 0x20, 0x26, 0x20, 0x57, 0x43, 0x48, 0x2e, 0x43, 0x4e];
var bootloaderDeviceID = null;
var bootloaderIDCmd = [
	0xA7, 0x02, 0x00, 0x1F, 0x00];
var bootloaderVersion = null;
var bootloaderID = null;
var bootloaderMask = Array(8).fill(0);
var bootloaderUploadReady = false;
var bootloaderInitCmd = [
    0xA8, 0x0E, 0x00, 0x07, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00, 0xFF, 0x52, 0x00, 0x00];
var bootloaderAddessCmd = [
	0xA3, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00];
var bootloaderEraseCmd = [
	0xA4, 0x01, 0x00, 0x08];
var bootloaderResetCmd = [
	0xA2, 0x01, 0x00, 0x01]; // if 0x00 not run, 0x01 run
var bootloaderWriteCmd = Array(64).fill(0);
bootloaderWriteCmd[0] = 0xA5;
bootloaderWriteCmd[1] = 0x3D;
var bootloaderVerifyCmd = Array(64).fill(0);
bootloaderVerifyCmd[0] = 0xA6;
bootloaderVerifyCmd[1] = 0x3D;


async function connectCH55xBootloader() {
    let filters = [{
            'vendorId': 0x4348
            , 'productId': 0x55E0
        }] //ch55x bootloader
    try {
        device = await navigator.usb.requestDevice({
            'filters': filters
        });
    } catch (error) {
        if (typeof this.errorCallback !== "undefined") {
            this.errorCallback(error);
        }
        if (error.message.indexOf('Must be handling a user gesture to show a permission request') >= 0) {
            statusDiv.innerHTML = 'Chrome need open function be triggered with user gesture. Try to call it in mouseClicked() or keyPressed(), etc.';
        } else if (error.message.indexOf('No device selected') >= 0) {
            statusDiv.innerHTML = 'No device selected';
        } else {
            statusDiv.innerHTML = error
        }
        return;
    }
    await device.open();
    if (device.configuration === null) {
        await device.selectConfiguration(1);
    }
    var configurationInterfaces = device.configuration.interfaces;
    //find interface with Class #0xFF (vendor specified), if multiple exists, the last one will be used.
    configurationInterfaces.forEach(element => {
        element.alternates.forEach(elementalt => {
            if (elementalt.interfaceClass == 0xff) {
                interfaceNumber = element.interfaceNumber;
                elementalt.endpoints.forEach(elementendpoint => {
                    if (elementendpoint.direction == "out") {
                        endpointOut = elementendpoint.endpointNumber;
                    }
                    if (elementendpoint.direction == "in") {
                        endpointIn = elementendpoint.endpointNumber;
                    }
                })
            }
        })
    })

    await device.claimInterface(interfaceNumber)

    //detect MCU
    await device.transferOut(endpointOut, (new Uint8Array(bootloaderDetectCmd)).buffer)
    result = await device.transferIn(endpointIn, 64);
    resultUint8 = new Uint8Array(result.data.buffer)
    bootloaderDeviceID = resultUint8[4]
    if (resultUint8[5] != 0x11) {
        statusDiv.innerHTML = 'MCU family Not support';
        return 0;
    }
    if (![0x51, 0x52, 0x54, 0x58, 0x59].includes(bootloaderDeviceID)) {
        statusDiv.innerHTML = 'Device not supported 0x' + bootloaderDeviceID.toString(16);
        return 0;
    }

    //detect ID and bootloader
    await device.transferOut(endpointOut, (new Uint8Array(bootloaderIDCmd)).buffer)
    result = await device.transferIn(endpointIn, 64);
    resultUint8 = new Uint8Array(result.data.buffer)
    bootloaderVersion = resultUint8[19] + '.' + resultUint8[20] + '.' + resultUint8[21];
    let bootloaderVersionNumber = resultUint8[19] * 100 + resultUint8[20] * 10 + resultUint8[21];
    if (bootloaderVersionNumber < 231 || bootloaderVersionNumber > 250 ) {
        statusDiv.innerHTML = 'bootloader Version not supported: ' + bootloaderVersion;
        return 0;
    }
    bootloaderID = [resultUint8[22], resultUint8[23], resultUint8[24], resultUint8[25]];
    statusDiv.innerHTML = 'bootloader Version ' + bootloaderVersion + ', ID: ' + bootloaderID.join(', ');

    var idSum = (bootloaderID[0] + bootloaderID[1] + bootloaderID[2] + bootloaderID[3]) & 0xFF;
    for (i = 0; i < 8; ++i) {
        bootloaderMask[i] = idSum;
    }
    bootloaderMask[7] = (bootloaderMask[7] + bootloaderDeviceID) & 0xFF;
    var maskStr = 'XOR Mask: ';
    for (i = 0; i < 8; ++i) {
        maskStr = maskStr + bootloaderMask[i].toString(16) + ' ';
    }
    console.log(maskStr);
    bootloaderUploadReady = true;
}

async function uploadCH55xBootloader(hexContent) {
    //console.log(hexContent);

    //send init cmd
    await device.transferOut(endpointOut, (new Uint8Array(bootloaderInitCmd)).buffer)
    result = await device.transferIn(endpointIn, 64);
    //console.log(new Uint8Array(result.data.buffer))

    //detect ID and bootloader
    await device.transferOut(endpointOut, (new Uint8Array(bootloaderIDCmd)).buffer)
    result = await device.transferIn(endpointIn, 64);
    //console.log(new Uint8Array(result.data.buffer))

    // Set Flash Address to 0
    await device.transferOut(endpointOut, (new Uint8Array(bootloaderAddessCmd)).buffer)
    result = await device.transferIn(endpointIn, 64);
    //console.log(new Uint8Array(result.data.buffer))

    // Erase
    await device.transferOut(endpointOut, (new Uint8Array(bootloaderEraseCmd)).buffer)
    result = await device.transferIn(endpointIn, 64);
    //console.log(new Uint8Array(result.data.buffer))

    console.log('Write ' + hexContent.length + ' bytes from bin file.');
    // Progress
    var writeDataSize, totalPackets, lastPacketSize;
    writeDataSize = hexContent.length;
    totalPackets = parseInt((writeDataSize + 55) / 56);
    lastPacketSize = writeDataSize % 56;
    //make it multiple of 8
    lastPacketSize = parseInt((lastPacketSize + 7) / 8 * 8);
    if (lastPacketSize == 0) lastPacketSize = 56;

    //write
    for (i = 0; i < totalPackets; ++i) {
        var u16Tmp;
        var j;
        for (j = 0; j < 56; j++) {
            bootloaderWriteCmd[8 + j] = hexContent[i * 56 + j]
        }
        for (j = 0; j < 7; ++j) {
            for (var ii = 0; ii < 8; ++ii) {
                bootloaderWriteCmd[8 + j * 8 + ii] ^= bootloaderMask[ii];
            }
        }
        u16Tmp = i * 56;
        bootloaderWriteCmd[1] = 61 - (i < (totalPackets - 1) ? 0 : (56 - lastPacketSize)); //last packet can be smaller
        bootloaderWriteCmd[3] = u16Tmp & 0xFF;
        bootloaderWriteCmd[4] = (u16Tmp >> 8) & 0xFF;

        await device.transferOut(endpointOut, (new Uint8Array(bootloaderWriteCmd.slice(0, bootloaderWriteCmd[1] + 3))).buffer)
        result = await device.transferIn(endpointIn, 64);
        //console.log(new Uint8Array(result.data.buffer))

        statusDiv.innerHTML = 'flash package ' + (i + 1) + ' of ' + totalPackets;
    }

    //verify
    for (i = 0; i < totalPackets; ++i) {
        var u16Tmp;
        var j;
        for (j = 0; j < 56; j++) {
            bootloaderVerifyCmd[8 + j] = hexContent[i * 56 + j]
        }
        for (j = 0; j < 7; ++j) {
            for (var ii = 0; ii < 8; ++ii) {
                bootloaderVerifyCmd[8 + j * 8 + ii] ^= bootloaderMask[ii];
            }
        }
        u16Tmp = i * 56;
        bootloaderVerifyCmd[1] = 61 - (i < (totalPackets - 1) ? 0 : (56 - lastPacketSize)); //last packet can be smaller
        bootloaderVerifyCmd[3] = u16Tmp & 0xFF;
        bootloaderVerifyCmd[4] = (u16Tmp >> 8) & 0xFF;

        await device.transferOut(endpointOut, (new Uint8Array(bootloaderWriteCmd.slice(0, bootloaderVerifyCmd[1] + 3))).buffer)
        result = await device.transferIn(endpointIn, 64);
        resultUint8 = new Uint8Array(result.data.buffer)

        if (resultUint8[4] != 0 || resultUint8[5] != 0) {
            statusDiv.innerHTML = 'Packet ' + i + ' does not match';
            return 0;
        } else {
            statusDiv.innerHTML = 'verify package ' + (i + 1) + ' of ' + totalPackets;
        }
    }

    await device.transferOut(endpointOut, (new Uint8Array(bootloaderResetCmd)).buffer)

    statusDiv.innerHTML = 'Flash Finished';
}


async function pressUpload() {
    if (!bootloaderUploadReady) {
        statusDiv.innerHTML = 'Connect bootloader first.';
        return 0;
    }
    // Check for the various File API support.
    if (window.File && window.FileReader && window.FileList && window.Blob) {
        // Great success! All the File APIs are supported.
        document.getElementById('fileid').click();
    } else {
        statusDiv.innerHTML = 'The File APIs are not fully supported in this browser.';
        return 0;
    }
}

async function handleFileSelect(evt) {
    var files = evt.target.files; // FileList object
    if (files.length > 0) {
        //don't care type

        var reader = new FileReader();
        // Closure to capture the file information.
        reader.onload = (function (theFile) {
            return function (e) {
                var hexContent = e.target.result
                uploadCH55xBootloader(parseIntelHex(hexContent, 63 * 1024).data)
            };
        })(files[0]);
        reader.readAsText(files[0]);
    }
}

//https://github.com/bminer/intel-hex.js/blob/master/index.js
//with modification

//Intel Hex record types
const DATA = 0
    , END_OF_FILE = 1
    , EXT_SEGMENT_ADDR = 2
    , START_SEGMENT_ADDR = 3
    , EXT_LINEAR_ADDR = 4
    , START_LINEAR_ADDR = 5;

const EMPTY_VALUE = 0xFF;

/* intel_hex.parse(data)
	`data` - Intel Hex file (string in ASCII format or Buffer Object)
	`bufferSize` - the size of the Buffer containing the data (optional)
	
	returns an Object with the following properties:
		- data - data as a Buffer Object, padded with 0xFF
			where data is empty.
		- startSegmentAddress - the address provided by the last
			start segment address record; null, if not given
		- startLinearAddress - the address provided by the last
			start linear address record; null, if not given
	Special thanks to: http://en.wikipedia.org/wiki/Intel_HEX
*/
function parseIntelHex(data, bufferSize) {
    //Initialization
    var buf = Array(bufferSize).fill(0)
        , bufLength = 0, //Length of data in the buffer
        highAddress = 0, //upper address
        startSegmentAddress = null
        , startLinearAddress = null
        , lineNum = 0, //Line number in the Intel Hex string
        pos = 0; //Current position in the Intel Hex string
    const SMALLEST_LINE = 11;
    while (pos + SMALLEST_LINE <= data.length) {
        //Parse an entire line
        if (data.charAt(pos++) != ":")
            throw new Error("Line " + (lineNum + 1) +
                " does not start with a colon (:).");
        else
            lineNum++;
        //Number of bytes (hex digit pairs) in the data field
        var dataLength = parseInt(data.substr(pos, 2), 16);
        pos += 2;
        //Get 16-bit address (big-endian)
        var lowAddress = parseInt(data.substr(pos, 4), 16);
        pos += 4;
        //Record type
        var recordType = parseInt(data.substr(pos, 2), 16);
        pos += 2;
        //Data field (hex-encoded string)
        var dataField = data.substr(pos, dataLength * 2);
        var dataFieldBuf = [];
        for (i = 0; i < dataField.length / 2; i++) {
            dataFieldBuf.push(parseInt(dataField.substring(i * 2, i * 2 + 2), 16) & 0xFF)
        }
        pos += dataLength * 2;
        //Checksum
        var checksum = parseInt(data.substr(pos, 2), 16);
        pos += 2;
        //Validate checksum
        var calcChecksum = (dataLength + (lowAddress >> 8) +
            lowAddress + recordType) & 0xFF;
        for (var i = 0; i < dataLength; i++)
            calcChecksum = (calcChecksum + dataFieldBuf[i]) & 0xFF;
        calcChecksum = (0x100 - calcChecksum) & 0xFF;
        if (checksum != calcChecksum)
            throw new Error("Invalid checksum on line " + lineNum +
                ": got " + checksum + ", but expected " + calcChecksum);
        //Parse the record based on its recordType
        switch (recordType) {
        case DATA:
            var absoluteAddress = highAddress + lowAddress;
            //Expand buf, if necessary
            if (absoluteAddress + dataLength >= buf.length) {
                var tmp = Buffer.alloc((absoluteAddress + dataLength) * 2);
                buf.copy(tmp, 0, 0, bufLength);
                buf = tmp;
            }
            //Write over skipped bytes with EMPTY_VALUE
            if (absoluteAddress > bufLength)
                buf.fill(EMPTY_VALUE, bufLength, absoluteAddress);
            //Write the dataFieldBuf to buf
            for (var i = 0; i < dataFieldBuf.length; i++) {
                buf[i + absoluteAddress] = dataFieldBuf[i];
            }
            bufLength = Math.max(bufLength, absoluteAddress + dataLength);
            break;
        case END_OF_FILE:
            if (dataLength != 0)
                throw new Error("Invalid EOF record on line " +
                    lineNum + ".");
            return {
                "data": buf.slice(0, bufLength)
                , "startSegmentAddress": startSegmentAddress
                , "startLinearAddress": startLinearAddress
            };
            break;
        case EXT_SEGMENT_ADDR:
            if (dataLength != 2 || lowAddress != 0)
                throw new Error("Invalid extended segment address record on line " +
                    lineNum + ".");
            highAddress = parseInt(dataField, 16) << 4;
            break;
        case START_SEGMENT_ADDR:
            if (dataLength != 4 || lowAddress != 0)
                throw new Error("Invalid start segment address record on line " +
                    lineNum + ".");
            startSegmentAddress = parseInt(dataField, 16);
            break;
        case EXT_LINEAR_ADDR:
            if (dataLength != 2 || lowAddress != 0)
                throw new Error("Invalid extended linear address record on line " +
                    lineNum + ".");
            highAddress = parseInt(dataField, 16) << 16;
            break;
        case START_LINEAR_ADDR:
            if (dataLength != 4 || lowAddress != 0)
                throw new Error("Invalid start linear address record on line " +
                    lineNum + ".");
            startLinearAddress = parseInt(dataField, 16);
            break;
        default:
            throw new Error("Invalid record type (" + recordType +
                ") on line " + lineNum);
            break;
        }
        //Advance to the next line
        if (data.charAt(pos) == "\r")
            pos++;
        if (data.charAt(pos) == "\n")
            pos++;
    }
    throw new Error("Unexpected end of input: missing or invalid EOF record.");
};

function dropHandler(ev) {
    console.log('File(s) dropped');

    // Prevent default behavior (Prevent file from being opened)
    ev.preventDefault();

    hexFile = undefined;

    if (ev.dataTransfer.items) {
        // Use DataTransferItemList interface to access the file(s)
        for (var i = 0; i < ev.dataTransfer.items.length; i++) {
            // If dropped items aren't files, reject them
            if (ev.dataTransfer.items[i].kind === 'file') {
                var file = ev.dataTransfer.items[i].getAsFile();
                if (i == 0) {
                    hexFile = file;
                }
            }
        }
    } else {
        // Use DataTransfer interface to access the file(s)
        for (var i = 0; i < ev.dataTransfer.files.length; i++) {
            if (i == 0) {
                hexFile = ev.dataTransfer.files[i];
            }
        }
    }

    if (hexFile) {
        var reader = new FileReader();
        // Closure to capture the file information.
        reader.onload = (function (theFile) {
            return function (e) {
                var hexContent = e.target.result
                uploadCH55xBootloader(parseIntelHex(hexContent, 63 * 1024).data)
            };
        })(file);
        reader.readAsText(file);
    }

}

function dragOverHandler(ev) {
    //console.log('File(s) in drop zone'); 

    // Prevent default behavior (Prevent file from being opened)
    ev.preventDefault();
}

document.addEventListener('DOMContentLoaded', function () {
    let connectButton = document.querySelector('#connect');
    let uploadButton = document.querySelector('#upload');
    let dropZoneDiv = document.querySelector('#drop_zone');
    statusDiv = document.querySelector('#status');

    connectButton.addEventListener('click', connectCH55xBootloader);
    uploadButton.addEventListener('click', pressUpload);
    dropZoneDiv.addEventListener('drop', dropHandler, false);
    dropZoneDiv.addEventListener('dragover', dragOverHandler, false)

    document.getElementById('fileid').addEventListener('change', handleFileSelect, false);
}, false);