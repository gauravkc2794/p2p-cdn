/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2007 University of Washington
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/packet-socket-address.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "udp-peer-client.h"
#include "udp-cdn-server.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iterator>

namespace ns3 {

    NS_LOG_COMPONENT_DEFINE ("UdpCDNServerApplication");

    NS_OBJECT_ENSURE_REGISTERED (UdpCDNServer);

    std::vector<Address> addList;

    TypeId
        UdpCDNServer::GetTypeId (void)
        {
            static TypeId tid = TypeId ("ns3::UdpCDNServer")
                .SetParent<Application> ()
                .SetGroupName("Applications")
                .AddConstructor<UdpCDNServer> ()
                .AddAttribute ("Port", "Port on which we listen for incoming packets.",
                        UintegerValue (9),
                        MakeUintegerAccessor (&UdpCDNServer::m_port),
                        MakeUintegerChecker<uint16_t> ())
                .AddTraceSource ("Rx", "A packet has been received",
                        MakeTraceSourceAccessor (&UdpCDNServer::m_rxTrace),
                        "ns3::Packet::TracedCallback")
                .AddTraceSource ("RxWithAddresses", "A packet has been received",
                        MakeTraceSourceAccessor (&UdpCDNServer::m_rxTraceWithAddresses),
                        "ns3::Packet::TwoAddressTracedCallback")
                ;
            return tid;
        }

    UdpCDNServer::UdpCDNServer ()
    {
        NS_LOG_FUNCTION (this);
    }

    UdpCDNServer::~UdpCDNServer()
    {
        NS_LOG_FUNCTION (this);
        m_socket = 0;
        m_socket6 = 0;
    }

    void
        UdpCDNServer::DoDispose (void)
        {
            NS_LOG_FUNCTION (this);
            Application::DoDispose ();
        }

    void 
        UdpCDNServer::StartApplication (void)
        {
            NS_LOG_FUNCTION (this);

            if (m_socket == 0)
            {
                TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
                m_socket = Socket::CreateSocket (GetNode (), tid);
                InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
                if (m_socket->Bind (local) == -1)
                {
                    NS_FATAL_ERROR ("Failed to bind socket");
                }
                if (addressUtils::IsMulticast (m_local))
                {
                    Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket);
                    if (udpSocket)
                    {
                        // equivalent to setsockopt (MCAST_JOIN_GROUP)
                        udpSocket->MulticastJoinGroup (0, m_local);
                    }
                    else
                    {
                        NS_FATAL_ERROR ("Error: Failed to join multicast group");
                    }
                }
            }

            if (m_socket6 == 0)
            {
                TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
                m_socket6 = Socket::CreateSocket (GetNode (), tid);
                Inet6SocketAddress local6 = Inet6SocketAddress (Ipv6Address::GetAny (), m_port);
                if (m_socket6->Bind (local6) == -1)
                {
                    NS_FATAL_ERROR ("Failed to bind socket");
                }
                if (addressUtils::IsMulticast (local6))
                {
                    Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket6);
                    if (udpSocket)
                    {
                        // equivalent to setsockopt (MCAST_JOIN_GROUP)
                        udpSocket->MulticastJoinGroup (0, local6);
                    }
                    else
                    {
                        NS_FATAL_ERROR ("Error: Failed to join multicast group");
                    }
                }
            }

            m_socket->SetRecvCallback (MakeCallback (&UdpCDNServer::HandleRead, this));
            m_socket6->SetRecvCallback (MakeCallback (&UdpCDNServer::HandleRead, this));
        }

    void 
        UdpCDNServer::StopApplication ()
        {
            NS_LOG_FUNCTION (this);

            if (m_socket != 0) 
            {
                m_socket->Close ();
                m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
            }
            if (m_socket6 != 0) 
            {
                m_socket6->Close ();
                m_socket6->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
            }
        }

    void 
        UdpCDNServer::HandleRead (Ptr<Socket> socket)
        {
            NS_LOG_FUNCTION (this << socket);

            Ptr<Packet> packet, pktToSend;
            Address from;
            Address localAddress;
            while ((packet = socket->RecvFrom (from))) {
                socket->GetSockName (localAddress);
                m_rxTrace (packet);
                m_rxTraceWithAddresses (packet, from, localAddress);
                if (InetSocketAddress::IsMatchingType (from))
                {
                    cout << "At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " <<
                            InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                            InetSocketAddress::ConvertFrom (from).GetPort () << "\n";
                    NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " <<
                            InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                            InetSocketAddress::ConvertFrom (from).GetPort ());
                }
                else if (Inet6SocketAddress::IsMatchingType (from))
                {
                    NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " <<
                            Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
                            Inet6SocketAddress::ConvertFrom (from).GetPort ());
                }

                packet->RemoveAllPacketTags ();
                packet->RemoveAllByteTags ();

                uint8_t *buffer = new uint8_t[packet->GetSize ()];
                uint32_t size = packet->CopyData(buffer, packet->GetSize ());
                string str = (char *)buffer;
                cout << "CDNServer::Size of incoming packet: " << size << "\n";
                //cout << str << "\n";

                std::vector<string> tokens; 

                // stringstream class check1 
                std::stringstream check1(str); 

                string intermediate; 

                // Tokenizing w.r.t. space ' ' 
                while(getline(check1, intermediate, ';')) 
                { 
                    tokens.push_back(intermediate); 
                } 

                // Printing the token vector 
                for(unsigned int i = 0; i < tokens.size(); i++) { 
                    //cout << tokens[i] << '\n'; 
                }

                if(tokens[0] == "false") {	
                    if(tokens[1] == "true" || addList.size() == 0) {
                        bool present = false;
                        for(unsigned int i = 0; i < addList.size(); i++) {
                            if(addList[i] == from) {
                                present = true;
                                break;
                            }
                        }
                        if(!present){
                            addList.push_back(from);
                        }
                        cout << "CDNServer:: Providing data to client!\n";
                        string result = "cdn;pdata";
                        uint32_t numbytes = static_cast<uint32_t>(std::stoul(tokens[2]));
                        pktToSend = Create<Packet>((uint8_t*)result.c_str(), numbytes);
                    } else {
                        cout << "CDNServer:: Providing peer list to client!\n";
                        string result = "cdn;" + tokens[2] +  ";";

                        for(unsigned int i = 0; i < addList.size(); i++) {
                            //cout << "Sizeof address : "<< addList[i].GetSerializedSize() << "\n";
                            InetSocketAddress isa = InetSocketAddress::ConvertFrom(addList[i]);

                            //cout << "CDNServer::Address : " << isa << "\n";
                            std::stringstream ss("");
                            isa.GetIpv4().Print(ss);
                            string addStr = ss.str() + ":";
                            std::stringstream news;
                            news << isa.GetPort();
                            addStr += news.str();
                            cout << "CDNServer:: "<< addStr << "\n";
                            if(i < addList.size()-1) addStr += ";";
                            result += addStr;
                        }
                        //std::copy(addList.begin(), addList.end()-1, std::stream_iterator<Address>(result, ", "));
                        //cout << "CDNServer:: Result being sent : "<< result << "\n";
                        //cout << "CDNServer:: Sizeof result: " << sizeof(result) << " " << strlen(result.c_str()) << "\n";
                        pktToSend = Create<Packet> ((uint8_t*)result.c_str(), strlen(result.c_str()) + 1);

                    }

                    NS_LOG_LOGIC ("Echoing packet");
                    socket->SendTo (pktToSend, 0, from);


                    if (InetSocketAddress::IsMatchingType (from))
                    {
                        NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server sent " << pktToSend->GetSize () << " bytes to " <<
                                InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                                InetSocketAddress::ConvertFrom (from).GetPort ());
                    }
                    else if (Inet6SocketAddress::IsMatchingType (from))
                    {
                        NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server sent " << pktToSend->GetSize () << " bytes to " <<
                                Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
                                Inet6SocketAddress::ConvertFrom (from).GetPort ());
                    }
                } else  {
                    cout << "CDNServer:: Received ack, updating metadata!\n";
                        bool present = false;
                        for(unsigned int i = 0; i < addList.size(); i++) {
                            if(addList[i] == from) {
                                present = true;
                                break;
                             }
                        }
                        if(!present){
                            addList.push_back(from);
                        }
                }
            }
        }

} // Namespace ns3
