/*
 * Bit.h
 *
 * Created: 4/22/2021 9:23:58 AM
 *  Author: xprusa06
 */ 


#pragma once

#define DDR(PORT) (*(&PORT - 1))
#define PIN(PORT) (*(&PORT - 2))

#define SET_BIT(REG,BIT) (REG |=  (1<<BIT))
#define CLR_BIT(REG,BIT) (REG &= ~(1<<BIT))

#define GET_BIT(REG,BIT) (REG & (1<<BIT))