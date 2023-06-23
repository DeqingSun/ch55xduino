/*
 * Copyright (c) 2013-2017 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ----------------------------------------------------------------------
 *
 * $Date:        1. December 2017
 * $Revision:    V2.0.0
 *
 * Project:      CMSIS-DAP Source
 * Title:        DAP.c CMSIS-DAP Commands
 *
 *---------------------------------------------------------------------------*/

#include "DAP.h"

// Get DAP Information
//   id:      info identifier
//   info:    pointer to info datas
//   return:  number of bytes in info datas
static uint8_t DAP_Info(uint8_t id, uint8_t *info) {
  uint8_t length = 0U;

  switch (id) {
  case DAP_ID_VENDOR:
    length = 0;
    break;
  case DAP_ID_PRODUCT:
    length = 0;
    break;
  case DAP_ID_SER_NUM:
    length = 0;
    break;
  case DAP_ID_FW_VER:
    length = (uint8_t)sizeof(DAP_FW_VER);
    memcpy(info, DAP_FW_VER, length);
    break;
  case DAP_ID_DEVICE_VENDOR:

    break;
  case DAP_ID_DEVICE_NAME:

    break;
  case DAP_ID_CAPABILITIES:
    info[0] = DAP_PORT_SWD;
    length = 1U;
    break;
  case DAP_ID_TIMESTAMP_CLOCK:

    break;
  case DAP_ID_SWO_BUFFER_SIZE:

    break;
  case DAP_ID_PACKET_SIZE:
    info[0] = (uint8_t)(DAP_PACKET_SIZE >> 0);
    info[1] = (uint8_t)(DAP_PACKET_SIZE >> 8);
    length = 2U;
    break;
  case DAP_ID_PACKET_COUNT:
    info[0] = DAP_PACKET_COUNT;
    length = 1U;
    break;
  default:
    break;
  }

  return (length);
}

// Process Delay command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
static uint8_t DAP_Delay(const uint8_t *req, uint8_t *res) {
  uint16_t delay;

  delay = (uint16_t)(*(req + 0)) | (uint16_t)(*(req + 1) << 8);

  while (--delay) {
  };

  *res = DAP_OK;
  return 1;
}

// Process Host Status command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
static uint8_t DAP_HostStatus(const uint8_t *req, uint8_t *res) {

  switch (*req) {
  case DAP_DEBUGGER_CONNECTED:
    LED = ((*(req + 1) & 1U));
    break;
  case DAP_TARGET_RUNNING:
    LED = ((*(req + 1) & 1U));
    break;
  default:
    *res = DAP_ERROR;
    return 1;
  }

  *res = DAP_OK;
  return 1;
}

// Process Connect command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
__idata uint8_t debug_port;
static uint8_t DAP_Connect(const uint8_t *req, uint8_t *res) {
  uint8_t port;

  if (*req == DAP_PORT_AUTODETECT) {
    port = DAP_DEFAULT_PORT;
  } else {
    port = *req;
  }

  switch (port) {
  case DAP_PORT_SWD:
    debug_port = DAP_PORT_SWD;
    PORT_SWD_SETUP();
    break;
  default:
    port = DAP_PORT_DISABLED;
    break;
  }

  *res = (uint8_t)port;
  return 1;
}

// Process Disconnect command and prepare response
//   response: pointer to response datas
//   return:   number of bytes in response
#define PORT_OFF() PORT_SWD_SETUP()
static uint8_t DAP_Disconnect(uint8_t *res) {

  debug_port = DAP_PORT_DISABLED;
  PORT_OFF();

  *res = DAP_OK;
  return (1U);
}

// Process Reset Target command and prepare response
//   response: pointer to response datas
//   return:   number of bytes in response
static uint8_t DAP_ResetTarget(uint8_t *res) {
  *(res + 1) = 0; // RESET_TARGET();
  *(res + 0) = DAP_OK;
  return 2;
}

// Process SWJ Pins command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
static uint8_t DAP_SWJ_Pins(const uint8_t *req, uint8_t *res) {
  uint8_t value;
  uint8_t select;
  uint16_t wait;

  value = (uint8_t) * (req + 0);
  select = (uint8_t) * (req + 1);
  wait = (uint16_t)(*(req + 2) << 0) | (uint16_t)(*(req + 3) << 8);
  if ((uint8_t)(*(req + 4)) || (uint8_t)(*(req + 5)))
    wait |= 0x8000;

  if ((select & DAP_SWJ_SWCLK_TCK_BIT) != 0U) {
    if ((value & DAP_SWJ_SWCLK_TCK_BIT) != 0U) {
      SWK = 1;
    } else {
      SWK = 0;
    }
  }
  if ((select & DAP_SWJ_SWDIO_TMS_BIT) != 0U) {
    if ((value & DAP_SWJ_SWDIO_TMS_BIT) != 0U) {
      SWD = 1;
    } else {
      SWD = 0;
    }
  }
  if ((select & DAP_SWJ_nRESET_BIT) != 0U) {
    RST = value >> DAP_SWJ_nRESET;
  }

  if (wait != 0U) {

    do {
      if ((select & DAP_SWJ_SWCLK_TCK_BIT) != 0U) {
        if ((value >> DAP_SWJ_SWCLK_TCK) ^ SWK) {
          continue;
        }
      }
      if ((select & DAP_SWJ_SWDIO_TMS_BIT) != 0U) {
        if ((value >> DAP_SWJ_SWDIO_TMS) ^ SWD) {
          continue;
        }
      }
      if ((select & DAP_SWJ_nRESET_BIT) != 0U) {
        if ((value >> DAP_SWJ_nRESET) ^ RST) {
          continue;
        }
      }
      break;
    } while (wait--);
  }

  value = ((uint8_t)SWK << DAP_SWJ_SWCLK_TCK) |
          ((uint8_t)SWD << DAP_SWJ_SWDIO_TMS) | (0 << DAP_SWJ_TDI) |
          (0 << DAP_SWJ_TDO) | (0 << DAP_SWJ_nTRST) |
          ((uint8_t)RST << DAP_SWJ_nRESET);

  *res = (uint8_t)value;

  return 1;
}

// Process SWJ Clock command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
__xdata uint32_t fast_clock;
__xdata uint8_t clock_delay;
static uint8_t DAP_SWJ_Clock(const uint8_t *req, uint8_t *res) {
  /**/
  fast_clock = *((uint32_t *)req);

  // Timer0 used for millis, Timer1 used for serial0
  // we can only use Timer2
  TR2 = 0;

  RCLK = 0;
  TCLK = 0;
  C_T2 = 0;

  // bTMR_CLK may be set by uart0, we keep it as is.
  T2MOD |= bTMR_CLK | bT2_CLK; // use Fsys for T2

  CP_RL2 = 0;

  if (fast_clock < (2 * F_CPU / 65536L)) {
    RCAP2L = 0;
    RCAP2H = 0;
  } else {
    uint16_t reloadValueT2 = (uint16_t)(65536L - ((F_CPU / 2) / fast_clock));
    RCAP2L = reloadValueT2 & 0xFF;
    RCAP2H = (reloadValueT2 >> 8) & 0xFF;
  }
  TL2 = RCAP2L;
  TH2 = RCAP2H;

  TF2 = 0;
  TR2 = 1;

  clock_delay = 0;

  *res = DAP_OK;
  return 1;
}

// Process SWJ Sequence command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
static uint8_t DAP_SWJ_Sequence(const uint8_t *req, uint8_t *res) {
  uint8_t count;

  count = *req++;
  if (count == 0U) {
    count = 255U;
  }

  SWJ_Sequence(count, req);
  *res = DAP_OK;

  return 1;
}

// Process SWD Configure command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
__idata uint8_t turnaround;
__idata uint8_t data_phase;
static uint8_t DAP_SWD_Configure(const uint8_t *req, uint8_t *res) {
  uint8_t value;

  value = *req;
  turnaround = (value & 0x03U) + 1U;
  data_phase = (value & 0x04U) ? 1U : 0U;

  *res = DAP_OK;
  return 1;
}

// Process SWD Sequence command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
static uint8_t DAP_SWD_Sequence(const uint8_t *req, uint8_t *res) {
  uint8_t sequence_info;
  uint8_t sequence_count;
  uint8_t request_count;
  uint8_t response_count;
  uint8_t count;

  *res++ = DAP_OK;

  request_count = 1U;
  response_count = 1U;

  sequence_count = *req++;
  while (sequence_count--) {
    sequence_info = *req++;
    count = sequence_info & SWD_SEQUENCE_CLK;
    if (count == 0U) {
      count = 64U;
    }
    count = (count + 7U) / 8U;
    if ((sequence_info & SWD_SEQUENCE_DIN) != 0U) {
      SWD = 1;
    } else {
      SWD = 1;
    }
    SWD_Sequence(sequence_info, req, res);
    if (sequence_count == 0U) {
      SWD = 1;
    }
    if ((sequence_info & SWD_SEQUENCE_DIN) != 0U) {
      request_count++;
      res += count;
      response_count += count;
    } else {
      req += count;
      request_count += count + 1U;
    }
  }

  return response_count;
}

// Process Transfer Configure command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
__idata uint8_t idle_cycles;
__idata uint16_t retry_count;
__idata uint16_t match_retry;
static uint8_t DAP_TransferConfigure(const uint8_t *req, uint8_t *res) {

  idle_cycles = *(req + 0);
  retry_count = (uint16_t) * (req + 1) | (uint16_t)(*(req + 2) << 8);
  match_retry = (uint16_t) * (req + 3) | (uint16_t)(*(req + 4) << 8);

  *res = DAP_OK;
  return 1;
}

// Process SWD Transfer command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)

__xdata uint8_t datas[4];
__idata uint8_t match_mask[4];
__idata uint8_t match_value[4];
__idata uint8_t DAP_TransferAbort = 0U;
__idata uint8_t request_count;
__idata uint8_t request_value;
__idata uint8_t response_count;
__idata uint8_t response_value;
__idata uint16_t retry;
static uint8_t DAP_SWD_Transfer(const uint8_t *req, uint8_t *res) {
  const uint8_t *request_head;
  uint8_t *response_head;

  uint8_t post_read;
  uint8_t check_write;

  request_head = req;

  response_count = 0U;
  response_value = 0U;
  response_head = res;
  res += 2;

  DAP_TransferAbort = 0U;

  post_read = 0U;
  check_write = 0U;

  req++; // Ignore DAP index

  request_count = *req++;
  for (; request_count != 0U; request_count--) {
    request_value = *req++;
    if ((request_value & DAP_TRANSFER_RnW) != 0U) {
      // Read registers
      if (post_read) {
        // Read was posted before
        retry = retry_count;
        if ((request_value & (DAP_TRANSFER_APnDP | DAP_TRANSFER_MATCH_VALUE)) ==
            DAP_TRANSFER_APnDP) {
          // Read previous AP datas and post next AP read
          do {
            response_value = SWD_Transfer(request_value, datas);
          } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
                   !DAP_TransferAbort);
        } else {
          // Read previous AP datas
          do {
            response_value = SWD_Transfer(DP_RDBUFF | DAP_TRANSFER_RnW, datas);
          } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
                   !DAP_TransferAbort);
          post_read = 0U;
        }
        if (response_value != DAP_TRANSFER_OK) {
          break;
        }
        // Store previous AP datas
        *res++ = (uint8_t)datas[0];
        *res++ = (uint8_t)datas[1];
        *res++ = (uint8_t)datas[2];
        *res++ = (uint8_t)datas[3];
      }
      if ((request_value & DAP_TRANSFER_MATCH_VALUE) != 0U) {
        // Read with value match
        match_value[0] = (uint8_t)(*(req + 0));
        match_value[1] = (uint8_t)(*(req + 1));
        match_value[2] = (uint8_t)(*(req + 2));
        match_value[3] = (uint8_t)(*(req + 3));
        req += 4;
        match_retry = match_retry;
        if ((request_value & DAP_TRANSFER_APnDP) != 0U) {
          // Post AP read
          retry = retry_count;
          do {
            response_value = SWD_Transfer(request_value, NULL);
          } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
                   !DAP_TransferAbort);
          if (response_value != DAP_TRANSFER_OK) {
            break;
          }
        }
        do {
          // Read registers until its value matches or retry counter expires
          retry = retry_count;
          do {
            response_value = SWD_Transfer(request_value, datas);
          } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
                   !DAP_TransferAbort);
          if (response_value != DAP_TRANSFER_OK) {
            break;
          }
        } while (((datas[0] & match_mask[0]) != match_value[0] ||
                  (datas[1] & match_mask[1]) != match_value[1] ||
                  (datas[2] & match_mask[2]) != match_value[2] ||
                  (datas[3] & match_mask[3]) != match_value[3]) &&
                 match_retry-- && !DAP_TransferAbort);
        if ((datas[0] & match_mask[0]) != match_value[0] ||
            (datas[1] & match_mask[1]) != match_value[1] ||
            (datas[2] & match_mask[2]) != match_value[2] ||
            (datas[3] & match_mask[3]) != match_value[3]) {
          response_value |= DAP_TRANSFER_MISMATCH;
        }
        if (response_value != DAP_TRANSFER_OK) {
          break;
        }
      } else {
        // Normal read
        retry = retry_count;
        if ((request_value & DAP_TRANSFER_APnDP) != 0U) {
          // Read AP registers
          if (post_read == 0U) {
            // Post AP read
            do {
              response_value = SWD_Transfer(request_value, NULL);
            } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
                     !DAP_TransferAbort);
            if (response_value != DAP_TRANSFER_OK) {
              break;
            }
            post_read = 1U;
          }
        } else {
          // Read DP register
          do {
            response_value = SWD_Transfer(request_value, datas);
          } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
                   !DAP_TransferAbort);
          if (response_value != DAP_TRANSFER_OK) {
            break;
          }

          // Store datas
          *res++ = datas[0];
          *res++ = datas[1];
          *res++ = datas[2];
          *res++ = datas[3];
        }
      }
      check_write = 0U;
    } else {
      // Write register
      if (post_read) {
        // Read previous datas
        retry = retry_count;
        do {
          response_value = SWD_Transfer(DP_RDBUFF | DAP_TRANSFER_RnW, datas);
        } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
                 !DAP_TransferAbort);
        if (response_value != DAP_TRANSFER_OK) {
          break;
        }
        // Store previous datas
        *res++ = datas[0];
        *res++ = datas[1];
        *res++ = datas[2];
        *res++ = datas[3];
        post_read = 0U;
      }
      // Load datas
      datas[0] = (uint8_t)(*(req + 0));
      datas[1] = (uint8_t)(*(req + 1));
      datas[2] = (uint8_t)(*(req + 2));
      datas[3] = (uint8_t)(*(req + 3));
      req += 4;
      if ((request_value & DAP_TRANSFER_MATCH_MASK) != 0U) {
        // Write match mask
        match_mask[0] = datas[0];
        match_mask[1] = datas[1];
        match_mask[2] = datas[2];
        match_mask[3] = datas[3];
        response_value = DAP_TRANSFER_OK;
      } else {
        // Write DP/AP register
        retry = retry_count;
        do {
          response_value = SWD_Transfer(request_value, datas);
        } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
                 !DAP_TransferAbort);
        if (response_value != DAP_TRANSFER_OK) {
          break;
        }

        check_write = 1U;
      }
    }
    response_count++;
    if (DAP_TransferAbort) {
      break;
    }
  }

  for (; request_count != 0U; request_count--) {
    // Process canceled requests
    request_value = *req++;
    if ((request_value & DAP_TRANSFER_RnW) != 0U) {
      // Read register
      if ((request_value & DAP_TRANSFER_MATCH_VALUE) != 0U) {
        // Read with value match
        req += 4;
      }
    } else {
      // Write register
      req += 4;
    }
  }

  if (response_value == DAP_TRANSFER_OK) {
    if (post_read) {
      // Read previous datas
      retry = retry_count;
      do {
        response_value = SWD_Transfer(DP_RDBUFF | DAP_TRANSFER_RnW, datas);
      } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
               !DAP_TransferAbort);
      if (response_value != DAP_TRANSFER_OK) {
        goto end;
      }
      // Store previous datas
      *res++ = (uint8_t)datas[0];
      *res++ = (uint8_t)datas[1];
      *res++ = (uint8_t)datas[2];
      *res++ = (uint8_t)datas[3];
    } else if (check_write) {
      // Check last write
      retry = retry_count;
      do {
        response_value = SWD_Transfer(DP_RDBUFF | DAP_TRANSFER_RnW, NULL);
      } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
               !DAP_TransferAbort);
    }
  }

end:
  *(response_head + 0) = (uint8_t)response_count;
  *(response_head + 1) = (uint8_t)response_value;

  return ((uint8_t)(res - response_head));
}

// Process Dummy Transfer command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
// static uint8_t DAP_Dummy_Transfer(const uint8_t *req, uint8_t *res)
//{
//  uint8_t *request_head;
//  uint8_t request_count;
//  uint8_t request_value;

//  request_head = req;

//  req++; // Ignore DAP index

//  request_count = *req++;

//  for (; request_count != 0U; request_count--)
//  {
//    // Process dummy requests
//    request_value = *req++;
//    if ((request_value & DAP_TRANSFER_RnW) != 0U)
//    {
//      // Read registers
//      if ((request_value & DAP_TRANSFER_MATCH_VALUE) != 0U)
//      {
//        // Read with value match
//        req += 4;
//      }
//    }
//    else
//    {
//      // Write registers
//      req += 4;
//    }
//  }

//  *(res + 0) = 0U; // res count
//  *(res + 1) = 0U; // res value

//  return 2;
//}

// Process Transfer command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
static uint8_t DAP_Transfer(const uint8_t *req, uint8_t *res) {
  uint8_t num = 0;

  //  switch (debug_port)
  //  {
  //  case DAP_PORT_SWD:
  num = DAP_SWD_Transfer(req, res);
  //    break;
  //  default:
  //    num = DAP_Dummy_Transfer(req, res);
  //    break;
  //  }

  return (num);
}

// Process SWD Transfer Block command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response
static uint8_t DAP_SWD_TransferBlock(const uint8_t *req, uint8_t *res) {

  uint8_t *response_head;
  response_count = 0U;
  response_value = 0U;
  response_head = res;
  res += 3;

  DAP_TransferAbort = 0U;

  req++; // Ignore DAP index

  request_count = (uint16_t)(*(req + 0) << 0) | (uint16_t)(*(req + 1) << 8);
  req += 2;
  if (request_count == 0U) {
    goto end;
  }

  request_value = *req++;
  if ((request_value & DAP_TRANSFER_RnW) != 0U) {
    // Read register block
    if ((request_value & DAP_TRANSFER_APnDP) != 0U) {
      // Post AP read
      retry = retry_count;
      do {
        response_value = SWD_Transfer(request_value, NULL);
      } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
               !DAP_TransferAbort);
      if (response_value != DAP_TRANSFER_OK) {
        goto end;
      }
    }
    while (request_count--) {
      // Read DP/AP register
      if ((request_count == 0U) &&
          ((request_value & DAP_TRANSFER_APnDP) != 0U)) {
        // Last AP read
        request_value = DP_RDBUFF | DAP_TRANSFER_RnW;
      }
      retry = retry_count;
      do {
        response_value = SWD_Transfer(request_value, datas);
      } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
               !DAP_TransferAbort);
      if (response_value != DAP_TRANSFER_OK) {
        goto end;
      }
      // Store datas
      *res++ = (uint8_t)datas[0];
      *res++ = (uint8_t)datas[1];
      *res++ = (uint8_t)datas[2];
      *res++ = (uint8_t)datas[3];
      response_count++;
    }
  } else {
    // Write register block
    while (request_count--) {
      // Load datas
      datas[0] = (uint8_t)(*(req + 0));
      datas[1] = (uint8_t)(*(req + 1));
      datas[2] = (uint8_t)(*(req + 2));
      datas[3] = (uint8_t)(*(req + 3));

      req += 4;
      // Write DP/AP register
      retry = retry_count;
      do {
        response_value = SWD_Transfer(request_value, datas);
      } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
               !DAP_TransferAbort);
      if (response_value != DAP_TRANSFER_OK) {
        goto end;
      }
      response_count++;
    }
    // Check last write
    retry = retry_count;
    do {
      response_value = SWD_Transfer(DP_RDBUFF | DAP_TRANSFER_RnW, NULL);
    } while ((response_value == DAP_TRANSFER_WAIT) && retry-- &&
             !DAP_TransferAbort);
  }

end:
  *(response_head + 0) = (uint8_t)(response_count >> 0);
  *(response_head + 1) = (uint8_t)(response_count >> 8);
  *(response_head + 2) = (uint8_t)response_value;

  return ((uint8_t)(res - response_head));
}

// Process Transfer Block command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
static uint8_t DAP_TransferBlock(const uint8_t *req, uint8_t *res) {
  uint32_t num; // https://github.com/DeqingSun/ch55xduino/issues/72

  switch (debug_port) {
  case DAP_PORT_SWD:
    num = DAP_SWD_TransferBlock(req, res);
    break;

  default:
    *(res + 0) = 0U; // res count [7:0]
    *(res + 1) = 0U; // res count[15:8]
    *(res + 2) = 0U; // res value
    num = 3U;
    break;
  }

  if ((*(req + 3) & DAP_TRANSFER_RnW) != 0U) {
    // Read registers block
    num |= 4U << 16;
  } else {
    // Write registers block
    num |= (4U + (((uint8_t)(*(req + 1)) | (uint8_t)(*(req + 2) << 8)) * 4))
           << 16;
  }

  return (num);
}

// Process SWD Write ABORT command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response
static uint8_t DAP_SWD_WriteAbort(const uint8_t *req, uint8_t *res) {
  // Load datas (Ignore DAP index)
  datas[0] = (uint8_t)(*(req + 1));
  datas[1] = (uint8_t)(*(req + 2));
  datas[2] = (uint8_t)(*(req + 3));
  datas[3] = (uint8_t)(*(req + 4));

  // Write Abort register
  SWD_Transfer(DP_ABORT, datas);

  *res = DAP_OK;
  return (1U);
}

// Process Write ABORT command and prepare response
//   request:  pointer to request datas
//   response: pointer to response datas
//   return:   number of bytes in response (lower 16 bits)
//             number of bytes in request (upper 16 bits)
static uint8_t DAP_WriteAbort(const uint8_t *req, uint8_t *res) {
  uint8_t num;

  switch (debug_port) {
  case DAP_PORT_SWD:
    num = DAP_SWD_WriteAbort(req, res);
    break;

  default:
    *res = DAP_ERROR;
    num = 1U;
    break;
  }
  return num;
}

// DAP Thread.
uint8_t DAP_Thread(void) {
  uint8_t num;
  uint8_t returnVal = 0;

  if (1) {
    uint8_t __xdata *req = &Ep1Buffer[0];
    uint8_t __xdata *res = &Ep1Buffer[64];

    *res++ = *req;
    switch (*req++) {
    case ID_DAP_Transfer:
      num = DAP_Transfer(req, res);
      break;

    case ID_DAP_TransferBlock:
      num = DAP_TransferBlock(req, res);
      break;

    case ID_DAP_Info:
      num = DAP_Info(*req, res + 1);
      *res = (uint8_t)num;
      num++;
      break;

    case ID_DAP_HostStatus:
      num = DAP_HostStatus(req, res);
      break;

    case ID_DAP_Connect:
      num = DAP_Connect(req, res);
      break;

    case ID_DAP_Disconnect:
      num = DAP_Disconnect(res);
      break;

    case ID_DAP_Delay:
      num = DAP_Delay(req, res);
      break;

    case ID_DAP_ResetTarget:
      num = DAP_ResetTarget(res);
      break;

    case ID_DAP_SWJ_Pins:
      num = DAP_SWJ_Pins(req, res);
      break;

    case ID_DAP_SWJ_Clock:
      num = DAP_SWJ_Clock(req, res);
      break;

    case ID_DAP_SWJ_Sequence:
      num = DAP_SWJ_Sequence(req, res);
      break;

    case ID_DAP_SWD_Configure:
      num = DAP_SWD_Configure(req, res);
      break;

    case ID_DAP_SWD_Sequence:
      num = DAP_SWD_Sequence(req, res);
      break;

    case ID_DAP_TransferConfigure:
      num = DAP_TransferConfigure(req, res);
      break;

    case ID_DAP_WriteABORT:
      num = DAP_WriteAbort(req, res);
      break;

    case ID_DAP_ExecuteCommands:
    case ID_DAP_QueueCommands:
    default:
      *(res - 1) = ID_DAP_Invalid;
      num = 1;
      break;
    }

    returnVal = num + 1;
  }

  return returnVal;
}
