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

namespace Leiftur.Ui
{
	public class SynthViewModel : ViewModelBase
	{
		private ModRoute[] modRoutes;

		private readonly OscTranceiver tranceiver;
		private Thread updateThread;
		private readonly DependencyObject[] Controls;
		private bool osc1Visible;
		private bool osc2Visible;
		private bool osc3Visible;
		private bool lfo1Visible;
		private bool lfo2Visible;
		private bool arpeggiatorVisible;
		private bool delayVisible;
		private bool chorusVisible;

		public SynthViewModel(Dictionary<DependencyObject, string> controls)
		{
			ModRoutes = Enumerable.Range(0, 12).Select(x => new ModRoute()).ToArray();
			tranceiver = new OscTranceiver(12003, 12005);

			this.Controls = controls.Keys.ToArray();
            RegisterControls();

			updateThread = new Thread(() => Listen()) { IsBackground = true };
			updateThread.Start();

			SetModuleVisibleCommand = new DelegateCommand(x => SetModuleVisible(x.ToString()), () => true);
			Osc1Visible = true;
			Lfo1Visible = true;
			DelayVisible = true;
		}
		
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

		private void RegisterControls()
		{
			foreach (var control in Controls)
			{
				var address = OscAddress.GetAddress(control);
				var modKnob = control as ModKnob;
                if (modKnob != null)
				{
					DependencyPropertyDescriptor.FromProperty(ModKnob.ValueProperty, typeof(ModKnob))
						.AddValueChanged(modKnob, (s, e) => { SendOscMessage(address, modKnob.Value); });
				}
			}
		}
		
		private void Listen()
		{
			while (true)
			{
				var msg = tranceiver.Receive();
				if (msg != null)
					ProcessOscMessage(msg);

				Thread.Sleep(20);
			}
		}

		private void ProcessOscMessage(byte[] msg)
		{
			
		}

		private void SendOscMessage(string address, double value)
		{
			Console.WriteLine("Sending {0} - {1}", address, value);
			var oscMsg = new SharpOSC.OscMessage(address, (float)value);
			var bytes = oscMsg.GetBytes();
			tranceiver.Send(bytes);
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
