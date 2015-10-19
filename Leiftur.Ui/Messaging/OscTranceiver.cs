using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace Leiftur.Ui.Messaging
{
	public class OscTranceiver
	{
		private readonly Socket sendSocket;
		private readonly IPEndPoint sendEp;

		private readonly UdpClient listener;
		private IPEndPoint receiveEp;

		public OscTranceiver(int sendPort, int receivePort)
		{
			sendSocket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
			sendEp = new IPEndPoint(IPAddress.Loopback, sendPort);

			listener = new UdpClient(receivePort);
			receiveEp = new IPEndPoint(IPAddress.Any, receivePort);
		}

		public void Send(byte[] bytes)
		{
			sendSocket.SendTo(bytes, sendEp);
		}

		public byte[] Receive()
		{
			if (listener.Available == 0)
				return null;

			var received = listener.Receive(ref receiveEp);
			return received;
		}
	}
}
