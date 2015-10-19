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
		}

	}
}
