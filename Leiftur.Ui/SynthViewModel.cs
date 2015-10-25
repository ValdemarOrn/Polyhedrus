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
		private readonly ControlManager controlManager;
		private readonly Dictionary<int, string> formattedParameters;

		private ModRoute[] modRoutes;
		private Module? activeModule;
		private int? activeParameter;

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
			controlManager = new ControlManager(controls, this);
            ModRoutes = Enumerable.Range(0, 12).Select(x => new ModRoute()).ToArray();
			formattedParameters = new Dictionary<int, string>();
			AnnouncerText = "";
			
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

		internal void ProcessParameterUpdate(Module module, int parameter, double value, string formattedParameter)
		{
			formattedParameters[Parameters.Pack(module, parameter)] = formattedParameter;
			if (module == activeModule && parameter == activeParameter)
				UpdateAnnouncer();
		}

		internal void SetActiveParameter(Module module, int parameter)
		{
			activeModule = module;
			activeParameter = parameter;
			UpdateAnnouncer();
		}

		public void ClearActiveParameter()
		{
			activeModule = null;
			activeParameter = null;
		}

		private void UpdateAnnouncer()
		{
			if (activeModule == null || activeParameter == null)
			{
				AnnouncerText = "";
				return;
			}

			var formattedString = formattedParameters.GetValueOrDefault(Parameters.Pack(activeModule.Value, activeParameter.Value), "");
            AnnouncerText = Parameters.PrettyPrint(activeModule.Value, activeParameter.Value) + ": " + formattedString;
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
