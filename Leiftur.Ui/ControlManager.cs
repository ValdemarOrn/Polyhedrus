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
		private readonly DependencyObject[] controls;

		private UIElement currentControl;

		public ControlManager(Dictionary<DependencyObject, string> controls, SynthViewModel vm)
		{
			this.vm = vm;
			tranceiver = new OscTranceiver(12003, 12004);
			sendMessages = new Dictionary<string, OscMessage>();
			this.controls = controls.Keys.ToArray();
			RegisterControls();

			var oscThread = new Thread(() => ProcessOscMessages()) { IsBackground = true };
			oscThread.Start();
		}

		private void RegisterControls()
		{
			foreach (var control in controls)
			{
				if (control is UIElement)
				{
					UIElement uiElem = control as UIElement;
					uiElem.MouseEnter += (s, e) => SetActiveControl(s as UIElement, true);
					uiElem.MouseLeave += (s, e) => SetActiveControl(s as UIElement, false);
				}

				var address = OscAddress.GetAddress(control);

				var modKnob = control as ModKnob;
				if (modKnob != null)
				{
					DependencyPropertyDescriptor.FromProperty(ModKnob.ValueProperty, typeof(ModKnob))
						.AddValueChanged(modKnob, (s, e) => { SendOscMessage(address, modKnob.Value); });
				}
			}
		}

		/// <summary>
		/// Update loop for OSC messaging
		/// </summary>
		private void ProcessOscMessages()
		{
			while (true)
			{
				var msg = tranceiver.Receive();
				if (msg != null)
					ProcessOscMessage(msg);

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

				Thread.Sleep(20);
			}
		}
		
		private void ProcessOscMessage(byte[] bytes)
		{
			try
			{
				var msg = OscPacket.GetPacket(bytes) as OscMessage;
				Console.WriteLine(msg.ToString());
				if (msg.Address == "/Control/ParameterResponse")
				{
					var module = (Module)(int)msg.Arguments[0];
					var parameter = (int)msg.Arguments[1];
					var floatVal = (float)msg.Arguments[2];
					var formattedString = (string)msg.Arguments[3];
					
					vm.ProcessParameterUpdate(module, parameter, floatVal, formattedString.Trim());
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.GetTrace());
			}
		}

		private void SetActiveControl(UIElement uiElement, bool isActive)
		{
			if (isActive)
			{
				currentControl = uiElement;
				var address = OscAddress.GetAddress(uiElement);
				var tuple = Parameters.ParseAddress(address);
				vm.SetActiveParameter(tuple.Item1, tuple.Item2);
				
			}
			else if (currentControl.Equals(uiElement))
			{
				currentControl = null;
				vm.ClearActiveParameter();
			}
		}

		private void SendOscMessage(string address, double value)
		{
			Console.WriteLine("Sending {0} - {1}", address, value);
			var oscMsg = new OscMessage(address, (float)value);

			lock (sendMessages)
			{
				sendMessages[oscMsg.Address] = oscMsg;
			}
		}


	}
}
