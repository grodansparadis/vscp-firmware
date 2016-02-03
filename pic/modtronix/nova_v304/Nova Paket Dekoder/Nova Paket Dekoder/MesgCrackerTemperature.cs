/// 
/// ***************************************************************************
/// Copyright (c) 2007 and beyond Robert M. Bouwens, CH-5425 Schneisingen, Switzerland. Allrights reserved.
/// 
/// Written by Robert M. Bouwens, 
/// 
/// This code is free for private use.
/// Use in commercial applications requires written permission
/// This software is provided "as is", with no warranty.
/// 
/// For comments mailto:bouwens.mehl@bluewin.com
///
using System;
using System.Collections.Generic;
using System.Text;

namespace BRM_vscp_message
{
    class CMsgCrackerTemperature : CMsgCracker
    {
        public enum eTemperatureFormat : byte
        {
            Kelvin = 0,
            Celsius = 1,
            Fahrenheit = 2
        }
        public enum eDataFormat : byte
        {
            Bit = 0,
            Byte,
            String,
            Integer,
            Normalized_Integer,
            Floating_point_value ,
            Reserved,
            Reserved_II
        }

        public CMsgCrackerTemperature()
        {
        }

        public override string ToString(byte[] i_dataBytes)
        {
            string sTemperature = string.Empty;

            if (i_dataBytes.Length > 3)
            {
                int index = (i_dataBytes[0] & 0x07) + 1;
                int value = (i_dataBytes[0] >> 3) & 0x03;
                int count = i_dataBytes[1];
                int dFormat = (i_dataBytes[0] >> 5) & 0x07;

                eTemperatureFormat format = (eTemperatureFormat)value;
                eDataFormat dateFormat = (eDataFormat)dFormat;
                StringBuilder sb = new StringBuilder();

                value = i_dataBytes[2];
                if (dateFormat == eDataFormat.Normalized_Integer)
                {
                    if ((value & 128) > 0)
                        sTemperature = "-";
                    value &= 127;
                    value = (value << 8) + i_dataBytes[3];
                    double divider = Math.Pow(10, count);

                    double dTemp = value / divider;
                    sTemperature += dTemp.ToString();

                    sb.AppendFormat("Temperature for sensor={0} is {1} {2} data:", index, sTemperature, format);
                }
                else
                {
                    // do you do the other data formats?
                    sb.AppendFormat("Formated data is {0} ", dateFormat);
                }

                string sBytes = "{";
                for (int i = 0; i < i_dataBytes.Length; i++)
                {
                    sb.AppendFormat("{0:X2} ", i_dataBytes[i]);
                }

                sBytes += sb.ToString();
                // Kosmetik
                sBytes.TrimEnd(new char[] { ' ' });
                sBytes += "}";

                return sBytes;
            }
            else
            {
                sTemperature = "CMsgCrackerTemperature: not enough data bytes";
                return sTemperature;
            }
        }
    }
}
