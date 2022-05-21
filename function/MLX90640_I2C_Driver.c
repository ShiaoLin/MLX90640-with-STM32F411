/**
 * @copyright (C) 2017 Melexis N.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "MLX90640_I2C_Driver.h"



void MLX90640_I2CInit()
{

}

int MLX90640_I2CGeneralReset(void)
{
  uint8_t cmd[2] = { 0,0 };

  cmd[0] = 0x00;
  cmd[1] = 0x06;

  do
  {
    if (HAL_I2C_Master_Transmit(&hi2c1, cmd[0], &cmd[1], 1, 1000) != HAL_OK)
    {
      return -1;
    }
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
    {
    }
  } while (HAL_I2C_GetError(&hi2c1) == HAL_I2C_ERROR_AF);

  Delay_us(50);

  return 0;
}

int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t* data)
{
  uint8_t* p = (uint8_t*)data;

  if (HAL_I2C_Mem_Read(&hi2c1, (slaveAddr << 1), startAddress, I2C_MEMADD_SIZE_16BIT, p, nMemAddressRead * 2, 1000) != HAL_OK)
  {
    return -1;
  }
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
  {
  }

  for (int cnt = 0; cnt < nMemAddressRead * 2; cnt += 2)
  {
    uint8_t tempBuffer = p[cnt + 1];
    p[cnt + 1] = p[cnt];
    p[cnt] = tempBuffer;
  }
  return 0;
}

int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)
{
  uint8_t cmd[2] = { 0,0 };
  static uint16_t datacheck;

  cmd[0] = data >> 8;
  cmd[1] = data & 0x00FF;

  if (HAL_I2C_Mem_Write(&hi2c1, slaveAddr << 1, writeAddress, I2C_MEMADD_SIZE_16BIT, cmd, 2, 1000) != HAL_OK)
  {
    return -1;
  }
  else
  {
    MLX90640_I2CRead(slaveAddr, writeAddress, 1, &datacheck);
    if (datacheck != data)
    {
      return -2;
    }
  }
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
  {
  }

  return 0;
}

