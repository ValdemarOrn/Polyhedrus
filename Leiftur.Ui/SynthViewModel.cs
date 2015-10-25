using Leiftur.Ui.Messaging;
using LowProfile.Core.Ui;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using Leiftur.Ui.Components;
using LowProfile.Core.Extensions;
using SharpOSC;

namespace Leiftur.Ui
{
	public class SynthViewModel : ViewModelBase
	{
		private ModRoute[] modRoutes;

		private readonly OscTranceiver tranceiver;
		private readonly Dictionary<string, OscMessage> sendMessages;
		private Dictionary<int, string> formattedParameters;
		private readonly DependencyObject[] controls;
		private readonly Thread oscThread;

		private UIElement currentControl;
		private Module activeModule;
		private int activeParameter;

		private bool osc1Visible;
		private bool osc2Visible;
		private bool osc3Visible;
		private bool lfo1Visible;
		private bool lfo2Visible;
		private bool arpeggiatorVisible;
		private bool delayVisible;
		private bool chorusVisible;
		private string announcerText;

		public SynthViewModel(Dictionary<DependencyObject, string> controls)
		{
			ModRoutes = Enumerable.Range(0, 12).Select(x => new ModRoute()).ToArray();
			tranceiver = new OscTranceiver(12003, 12004);
			sendMessages = new Dictionary<string, OscMessage>();
			formattedParameters = new Dictionary<int, string>();
			AnnouncerText = "";

            this.controls = controls.Keys.ToArray();
            RegisterControls();

			oscThread = new Thread(() => ProcessOscMessages()) { IsBackground = true };
			oscThread.Start();

			SetModuleVisibleCommand = new DelegateCommand(x => SetModuleVisible(x.ToString()), () => true);
			Osc1Visible = true;
			Lfo1Visible = true;
			DelayVisible = true;
		}

		#region Properties

		public DelegateCommand SetModuleVisibleCommand { get; private set; }

		public ModRoute[] ModRoutes
		{
			get { return modRoutes; }
			set { modRoutes = value; }
		}

		public bool Osc1Visible
		{
			get { return osc1Visible; }
			set { osc1Visible = value; NotifyPropertyChanged(); }
		}

		public bool Osc2Visible
		{
			get { return osc2Visible; }
			set { osc2Visible = value; NotifyPropertyChanged(); }
		}

		public bool Osc3Visible
		{
			get { return osc3Visible; }
			set { osc3Visible = value; NotifyPropertyChanged(); }
		}

		public bool Lfo1Visible
		{
			get { return lfo1Visible; }
			set { lfo1Visible = value; NotifyPropertyChanged(); }
		}

		public bool Lfo2Visible
		{
			get { return lfo2Visible; }
			set { lfo2Visible = value; NotifyPropertyChanged(); }
		}

		public bool ArpeggiatorVisible
		{
			get { return arpeggiatorVisible; }
			set { arpeggiatorVisible = value; NotifyPropertyChanged(); }
		}

		public bool DelayVisible
		{
			get { return delayVisible; }
			set { delayVisible = value; NotifyPropertyChanged(); }
		}

		public bool ChorusVisible
		{
			get { return chorusVisible; }
			set { chorusVisible = value; NotifyPropertyChanged(); }
		}

		public string AnnouncerText
		{
			get { return announcerText; }
			set { announcerText = value; NotifyPropertyChanged(); }
		}

		#endregion

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

		private void SetActiveControl(UIElement uiElement, bool isActive)
		{
			if (isActive)
			{
				currentControl = uiElement;
				var address = OscAddress.GetAddress(uiElement);
				var tuple = Parameters.ParseAddress(address);
				activeModule = tuple.Item1;
				activeParameter = tuple.Item2;
				UpdateAnnouncer();
			}
			else if (currentControl.Equals(uiElement))
			{
				currentControl = null;
				AnnouncerText = " ";
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
					formattedParameters[Parameters.Pack(module, parameter)] = formattedString.Trim();
					if (module == activeModule && parameter == activeParameter)
						UpdateAnnouncer();
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.GetTrace());
			}
		}

		private void UpdateAnnouncer()
		{
			var formattedString = formattedParameters.GetValueOrDefault(Parameters.Pack(activeModule, activeParameter), "");
            AnnouncerText = Parameters.PrettyPrint(activeModule, activeParameter) + ": " + formattedString;
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

		private void SetModuleVisible(string module)
		{
			if (module == "Osc1")
			{
				Osc2Visible = false;
				Osc3Visible = false;
				Osc1Visible = true;
			}
			else if (module == "Osc2")
			{
				Osc1Visible = false;
				Osc3Visible = false;
				Osc2Visible = true;
			}
			else if (module == "Osc3")
			{
				Osc1Visible = false;
				Osc2Visible = false;
				Osc3Visible = true;
			}
			else if (module == "Lfo1")
			{
				ArpeggiatorVisible = false;
				Lfo2Visible = false;
				Lfo1Visible = true;
			}
			else if (module == "Lfo2")
			{
				Lfo1Visible = false;
				ArpeggiatorVisible = false;
				Lfo2Visible = true;
			}
			else if (module == "Arpeggiator")
			{
				Lfo2Visible = false;
				Lfo1Visible = false;
				ArpeggiatorVisible = true;
			}
			else if (module == "Delay")
			{
				ChorusVisible = false;
				DelayVisible = true;
			}
			else if (module == "Chorus")
			{
				DelayVisible = false;
				ChorusVisible = true;
			}
		}

	}
}
