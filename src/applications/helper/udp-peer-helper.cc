/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "udp-peer-helper.h"
#include "ns3/udp-cdn-server.h"
#include "ns3/udp-peer-client.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

UdpCDNServerHelper::UdpCDNServerHelper (uint16_t port)
{
  m_factory.SetTypeId (UdpCDNServer::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
}

void 
UdpCDNServerHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
UdpCDNServerHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
UdpCDNServerHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
UdpCDNServerHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
UdpCDNServerHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<UdpCDNServer> ();
  node->AddApplication (app);

  return app;
}

UdpPeerClientHelper::UdpPeerClientHelper (Address address, uint16_t port)
{
  m_factory.SetTypeId (UdpPeerClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
  SetAttribute ("RemotePort", UintegerValue (port));
}

UdpPeerClientHelper::UdpPeerClientHelper (Address address)
{
  m_factory.SetTypeId (UdpPeerClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
}

void 
UdpPeerClientHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

void
UdpPeerClientHelper::SetFill (Ptr<Application> app, std::string fill)
{
  app->GetObject<UdpPeerClient>()->SetFill (fill);
}

void
UdpPeerClientHelper::SetFill (Ptr<Application> app, uint8_t fill, uint32_t dataLength)
{
  app->GetObject<UdpPeerClient>()->SetFill (fill, dataLength);
}

void
UdpPeerClientHelper::SetFill (Ptr<Application> app, uint8_t *fill, uint32_t fillLength, uint32_t dataLength)
{
  app->GetObject<UdpPeerClient>()->SetFill (fill, fillLength, dataLength);
}

ApplicationContainer
UdpPeerClientHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
UdpPeerClientHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
UdpPeerClientHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
UdpPeerClientHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<UdpPeerClient> ();
  node->AddApplication (app);

  return app;
}

} // namespace ns3
