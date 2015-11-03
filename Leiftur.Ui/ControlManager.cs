using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using Leiftur.Ui.Components;
using Leiftur.Ui.Messaging;
using LowProfile.Core.Extensions;
using SharpOSC;

namespace Leiftur.Ui
{
	class ControlManager
	{
		private readonly SynthViewModel vm;

		private readonly OscTranceiver tranceiver;
		private readonly Dictionary<string, OscMessage> sendMessages;
		
		public ControlManager(SynthViewModel vm)
		{
			this.vm = vm;
			tranceiver = new OscTranceiver(12003, 12004);
			sendMessages = new Dictionary<string, OscMessage>();
			
			var oscThread = new Thread(() => ProcessOscMessages()) { IsBackground = true };
			oscThread.Start();
		}

		public void SendOscMessage(string address, double value)
		{
			Console.WriteLine("Sending {0} - {1}", address, value);
			var oscMsg = new OscMessage(address, (float)value);

			lock (sendMessages)
			{
				sendMessages[oscMsg.Address] = oscMsg;
			}
		}

		public void SendOscControlMessage(string address, params object[] values)
		{
			Console.WriteLine("Sending {0}", address);
			var oscMsg = new OscMessage(address, values);

			lock (sendMessages)
			{
				sendMessages[oscMsg.Address] = oscMsg;
			}
		}
		
		/// <summary>
		/// Update loop for OSC messaging
		/// </summary>
		private void ProcessOscMessages()
		{
			while (true)
			{
				try
				{
					while (true)
					{
						var msg = tranceiver.Receive();
						if (msg == null)
							break;

						ProcessOscMessage(msg);
					}

					lock (sendMessages)
					{
						var keys = sendMessages.Keys.ToArray();
						foreach (var key in keys)
						{
							try
							{
								var oscMsg = sendMessages[key];
								sendMessages.Remove(key);
								var bytes = oscMsg.GetBytes();
								tranceiver.Send(bytes);
							}
							catch (Exception ex)
							{
								Console.WriteLine(ex.GetTrace());
							}
						}
					}
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex.GetTrace());
				}

				Thread.Sleep(20);
			}
		}
		
		private void ProcessOscMessage(byte[] bytes)
		{
			try
			{
				var msg = OscPacket.GetPacket(bytes) as OscMessage;
				Console.WriteLine(msg.ToString());
				if (msg.Address.StartsWith("/Control/"))
					vm.ProcessControlMessage(msg);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.GetTrace());
			}
		}
	}
}
