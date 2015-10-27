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
		private readonly Dictionary<int, DependencyObject> controls;

		private ModRoute[] modRoutes;
		private Module? activeModule;
		private int? activeParameter;
		private UIElement currentControl;

		private bool osc1Visible;
		private bool osc2Visible;
		private bool osc3Visible;
		private bool lfo1Visible;
		private bool lfo2Visible;
		private bool arpeggiatorVisible;
		private bool delayVisible;
		private bool chorusVisible;
		private string announcerCaption;
		private string announcerValue;
		private bool disableSendValue;

		public SynthViewModel(Dictionary<DependencyObject, string> controlDict)
		{
			Application.Current.DispatcherUnhandledException += (s, e) =>
			{
				var ex = e.Exception;
				MessageBox.Show(ex.GetTrace(), "Error", MessageBoxButton.OK, MessageBoxImage.Error);
				e.Handled = true;
			};

			controlManager = new ControlManager(this);
            ModRoutes = Enumerable.Range(0, 8).Select(x => new ModRoute()).ToArray();
			formattedParameters = new Dictionary<int, string>();
			announcerCaption = "";
			announcerValue = "";

			this.controls = new Dictionary<int, DependencyObject>();
			RegisterControls(controlDict);

			SetModuleVisibleCommand = new DelegateCommand(x => SetModuleVisible(x.ToString()), () => true);
			SavePresetCommand = new DelegateCommand(x => SavePreset(), () => true);
			DeletePresetCommand = new DelegateCommand(x => DeletePreset(), () => true);
			Osc1Visible = true;
			Lfo1Visible = true;
			DelayVisible = true;
		}

		#region Properties

		public DelegateCommand SetModuleVisibleCommand { get; private set; }
		public DelegateCommand SavePresetCommand { get; private set; }
		public DelegateCommand DeletePresetCommand { get; private set; }

		public Window Parent { get; set; }

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

		public bool PresetVisible => !AnnouncerVisible;

		public bool AnnouncerVisible => !string.IsNullOrWhiteSpace(AnnouncerCaption);

		public string AnnouncerCaption
		{
			get { return announcerCaption; }
			set
			{
				announcerCaption = value;
				NotifyPropertyChanged();
				NotifyPropertyChanged(nameof(AnnouncerVisible));
				NotifyPropertyChanged(nameof(PresetVisible));
			}
		}

		public string AnnouncerValue
		{
			get { return announcerValue; }
			set
			{
				announcerValue = value;
				NotifyPropertyChanged();
				NotifyPropertyChanged(nameof(AnnouncerVisible));
				NotifyPropertyChanged(nameof(PresetVisible));
			}
		}

		#endregion
		
		internal void ProcessParameterUpdate(Module module, int parameter, double value, string formattedParameter)
		{
			var key = Parameters.Pack(module, parameter);
            formattedParameters[key] = formattedParameter;

			if (module == activeModule && parameter == activeParameter)
			{
				UpdateAnnouncer(); // update announcer for active control
				return;
			}

			var control = controls.GetValueOrDefault(key);
			if (control == null)
				return;

			control.Dispatcher.InvokeAsync(() =>
			{
				try
				{
					disableSendValue = true;
					if (control is ModKnob)
					{
						(control as ModKnob).Value = value;
					}
				}
				finally
				{
					disableSendValue = false;
				}
			});
		}
		
		internal void RequestState()
		{
			controlManager.SendOscControlMessage("/Control/RequestState");
		}

		private void RegisterControls(Dictionary<DependencyObject, string> controlDict)
		{
			foreach (var control in controlDict.Keys)
			{
				if (control as UIElement == null)
					continue;
				
				UIElement uiElem = control as UIElement;
				uiElem.MouseEnter += (s, e) => SetActiveControl(s as UIElement, true);
				uiElem.MouseLeave += (s, e) => SetActiveControl(s as UIElement, false);
				
				var address = OscAddress.GetAddress(control);
				var key = Parameters.Pack(address);
				controls[key] = control;

				var modKnob = control as ModKnob;
				if (modKnob != null)
				{
					DependencyPropertyDescriptor.FromProperty(ModKnob.ValueProperty, typeof(ModKnob))
						.AddValueChanged(modKnob, (s, e) => SendValue(address, modKnob.Value));
				}
			}
		}

		private void SendValue(string address, double value)
		{
			if (disableSendValue)
				return;

			controlManager.SendOscMessage(address, value);
		}

		private void SetActiveControl(UIElement uiElement, bool isActive)
		{
			if (isActive)
			{
				currentControl = uiElement;
				var address = OscAddress.GetAddress(uiElement);
				var tuple = Parameters.ParseAddress(address);
				SetActiveParameter(tuple.Item1, tuple.Item2);

			}
			else if (currentControl.Equals(uiElement))
			{
				currentControl = null;
				ClearActiveParameter();
			}
		}

		private void SetActiveParameter(Module module, int parameter)
		{
			activeModule = module;
			activeParameter = parameter;
			UpdateAnnouncer();
		}

		private void ClearActiveParameter()
		{
			activeModule = null;
			activeParameter = null;
			UpdateAnnouncer();
		}

		private void UpdateAnnouncer()
		{
			if (activeModule == null || activeParameter == null)
			{
				AnnouncerCaption = "";
				AnnouncerValue = "";
				return;
			}

			var formattedString = formattedParameters.GetValueOrDefault(Parameters.Pack(activeModule.Value, activeParameter.Value), "");
			AnnouncerCaption = Parameters.PrettyPrint(activeModule.Value, activeParameter.Value);
			AnnouncerValue = formattedString;
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
		
		private void SavePreset()
		{
			var presetName = new SavePresetDialog() { Owner = Parent }.ShowDialog("Preset Name");
			if (string.IsNullOrWhiteSpace(presetName))
				return;

			var bankName = "User Bank";
			if (!CheckValidName(bankName))
				throw new Exception("The selected bank name is invalid");
			if (!CheckValidName(presetName))
				throw new Exception("The selected preset name is invalid");

			controlManager.SendOscControlMessage("/Control/SavePreset", bankName, presetName);
		}

		private bool CheckValidName(string name)
		{
			if (name.Length > 256)
				return false;

			// ASCII encoding replaces non-ascii with question marks, so we use UTF8 to see if multi-byte sequences are there
			return name.All(x => (x >= '0' && x <= '9') || (x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z') || x == '-' || x == '_' || x == '(' || x == ')');
        }

		private void DeletePreset()
		{
			throw new NotImplementedException();
		}
	}
}
