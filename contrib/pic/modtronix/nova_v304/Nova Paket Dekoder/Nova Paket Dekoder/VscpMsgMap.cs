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
using Xedia.Protocol.VSCP;

namespace BRM_vscp_message
{
    /// <summary>
    /// Base class for the message decoder
    /// </summary>
    abstract class CMsgCracker
    {
        public abstract string ToString(byte[] i_dataPacket);

        // Derived classes have members which contain enough information
        // to 'make' the ToString operations for every specific message.
    }

    class CVscpMsgMap
    {
        /// <summary>
        /// A one-to-one mapping of event tags to delegates managed by the event pool.
        /// </summary>
        protected Dictionary<string, CMsgCracker> m_eventNameToHelperMap;
        
        public CVscpMsgMap()
        {
            m_eventNameToHelperMap = new Dictionary<string, CMsgCracker>();
            Publish(EventCode.Temperature.ToString(), new CMsgCrackerTemperature());
        }

        public void Publish(string i_EventName, CMsgCracker i_msgCracker)
        {
            m_eventNameToHelperMap.Add(i_EventName, i_msgCracker);
        }

        public string Decode(string i_eventName, byte[] i_dataBytes)
        {
            if (m_eventNameToHelperMap.ContainsKey(i_eventName))
                return m_eventNameToHelperMap[i_eventName].ToString(i_dataBytes);
            else
                return string.Empty;
        }
    }
}
