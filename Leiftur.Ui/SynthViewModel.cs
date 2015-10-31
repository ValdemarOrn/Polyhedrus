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
		private readonly Dictionary<string, List<string>> presetBanks;

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
		private string selectedBank;
		private string selectedPreset;
		private bool matrix1Visible;
		private bool matrix2Visible;
		private bool macrosVisible;
		private Dictionary<int, string> waveformTypes;
		private bool ampEnvPage2Visible;
		private bool filterEnvPage2Visible;

		public SynthViewModel(Dictionary<DependencyObject, string> controlDict)
		{
			Application.Current.DispatcherUnhandledException += (s, e) =>
			{
				var ex = e.Exception;
				MessageBox.Show(ex.GetTrace(), "Error", MessageBoxButton.OK, MessageBoxImage.Error);
				e.Handled = true;
			};

			presetBanks = new Dictionary<string, List<string>> { { "Default Bank", new List<string>() } };
			controlManager = new ControlManager(this);
            ModRoutes = Enumerable.Range(0, 8).Select(x => new ModRoute()).ToArray();
			formattedParameters = new Dictionary<int, string>();
			announcerCaption = "";
			announcerValue = "";

			this.controls = new Dictionary<int, DependencyObject>();
			RegisterControls(controlDict);

			ShowAmpEnvPage2 = new DelegateCommand(_ => AmpEnvPage2Visible = !AmpEnvPage2Visible);
			ShowFilterEnvPage2 = new DelegateCommand(_ => FilterEnvPage2Visible = !FilterEnvPage2Visible);
			SetBankCommand = new DelegateCommand(x => SelectedBank = x.ToString(), () => true);
			SetPresetCommand = new DelegateCommand(x => SelectedPreset = x.ToString(), () => true);
			SetModuleVisibleCommand = new DelegateCommand(x => SetModuleVisible(x.ToString()), () => true);
			SavePresetCommand = new DelegateCommand(x => SavePreset(), () => true);
			DeletePresetCommand = new DelegateCommand(x => DeletePreset(), () => true);
			Osc1Visible = true;
			Lfo1Visible = true;
			DelayVisible = true;
			Matrix1Visible = true;

			SelectedBank = "Unknown Bank";
			SelectedPreset = "Unknown Preset";
		}

		#region Properties

		public DelegateCommand ShowAmpEnvPage2 { get; private set; }
		public DelegateCommand ShowFilterEnvPage2 { get; private set; }
        public DelegateCommand SetBankCommand { get; private set; }
		public DelegateCommand SetPresetCommand { get; private set; }
		public DelegateCommand SetModuleVisibleCommand { get; private set; }
		public DelegateCommand SavePresetCommand { get; private set; }
		public DelegateCommand DeletePresetCommand { get; private set; }

		public Window Parent { get; set; }

		public string[] Banks => presetBanks.Keys.ToArray();

		public string[] Presets => (presetBanks.GetValueOrDefault(selectedBank) ?? new List<string>()).ToArray();

		public string SelectedBank
		{
			get { return selectedBank; }
			set
			{
				var isSame = value == selectedBank;
				selectedBank = value;
				NotifyPropertyChanged();
				if (!isSame)
					RequestPresets();
			}
		}

		public string SelectedPreset
		{
			get { return selectedPreset; }
			set
			{
				var isSame = value == SelectedPreset;
				selectedPreset = value;
				NotifyPropertyChanged();
				if (!isSame)
					LoadPreset();
			}
		}

		public ModRoute[] ModRoutes
		{
			get { return modRoutes; }
			set { modRoutes = value; }
		}

		public bool Osc1Visible
		{
			get { return osc1Visible; }
			set
			{
				if (value == osc1Visible)
					return;

				Osc2Visible = false;
				Osc3Visible = false;
				osc1Visible = value;
				NotifyPropertyChanged();
			}
		}

		public bool Osc2Visible
		{
			get { return osc2Visible; }
			set
			{
				if (value == osc2Visible)
					return;

				Osc1Visible = false;
				Osc3Visible = false;
				osc2Visible = value;
				NotifyPropertyChanged();
			}
		}

		public bool Osc3Visible
		{
			get { return osc3Visible; }
			set
			{
				if (value == osc3Visible)
					return;

				Osc1Visible = false;
				Osc2Visible = false;
				osc3Visible = value;
				NotifyPropertyChanged();
			}
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

		public bool Matrix1Visible
		{
			get { return matrix1Visible; }
			set { matrix1Visible = value; NotifyPropertyChanged(); }
		}

		public bool Matrix2Visible
		{
			get { return matrix2Visible; }
			set { matrix2Visible = value; NotifyPropertyChanged(); }
		}

		public bool MacrosVisible
		{
			get { return macrosVisible; }
			set { macrosVisible = value; NotifyPropertyChanged(); }
		}

		public bool AmpEnvPage2Visible
		{
			get { return ampEnvPage2Visible; }
			set { ampEnvPage2Visible = value; NotifyPropertyChanged(); }
		}

		public bool FilterEnvPage2Visible
		{
			get { return filterEnvPage2Visible; }
			set { filterEnvPage2Visible = value; NotifyPropertyChanged(); }
		}

		public string AnnouncerCaption
		{
			get { return announcerCaption; }
			set
			{
				announcerCaption = value;
				NotifyPropertyChanged();
			}
		}

		public string AnnouncerValue
		{
			get { return announcerValue; }
			set
			{
				announcerValue = value;
				NotifyPropertyChanged();
			}
		}

		#endregion

		internal void Initialize()
		{
			RequestBanks();
			RequestState();
		}

		internal void ProcessControlMessage(OscMessage msg)
		{
			if (msg.Address == "/Control/PresetData")
			{
				var serializedData = msg.Arguments[0] as string;
				if (serializedData == null)
					return;

				var data = serializedData
					.Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries)
					.Select(x => x.Split('='))
					.Where(x => x.Length == 2)
					.Select(x => new { Key = x[0], Value = x[1] })
					.ToDictionary(x => x.Key, x => x.Value);

				SelectedBank = data.GetValueOrDefault("BankName", "Unknown Bank");
				SelectedPreset = data.GetValueOrDefault("PresetName", "Unknown Preset");
			}
			else if(msg.Address == "/Control/ParameterData")
			{
				var module = (Module)(int)msg.Arguments[0];
				var parameter = (int)msg.Arguments[1];
				var floatVal = (float)msg.Arguments[2];
				var formattedString = (string)msg.Arguments[3];
				ProcessParameterUpdate(module, parameter, floatVal, formattedString.Trim());
			}
			else if (msg.Address == "/Control/Banks")
			{
				if (msg.Arguments.Length != 1 || !(msg.Arguments[0] is string))
					return;

				var bankString = msg.Arguments.First() as string;
				var banks = bankString.Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
				if (banks.Length == 0)
					return;

				presetBanks.Clear();
				foreach (var bank in banks)
					presetBanks[bank] = new List<string>();
				NotifyPropertyChanged(nameof(Banks));
			}
			else if (msg.Address == "/Control/Presets")
			{
				if (msg.Arguments.Length != 2 || !(msg.Arguments[0] is string) || !(msg.Arguments[1] is string))
					return;

				var bankName = msg.Arguments.First() as string;
				var presets = (msg.Arguments.Last() as string).Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries);

				if (!presetBanks.ContainsKey(bankName) || presets.Length == 0)
					return;

				presetBanks[bankName].Clear();
				foreach (var preset in presets)
					presetBanks[bankName].Add(preset);

				NotifyPropertyChanged(nameof(Presets));
			}
			else if (msg.Address == "/Control/PresetsChanged")
			{
				if (msg.Arguments.Length != 1 || !(msg.Arguments[0] is string))
					return;

				var bank = msg.Arguments.First() as string;
				controlManager.SendOscControlMessage("/Control/RequestPresets", bank);
			}
        }

		private void ProcessParameterUpdate(Module module, int parameter, double value, string formattedParameter)
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

		private void RequestState()
		{
			controlManager.SendOscControlMessage("/Control/RequestState");
		}

		private void RequestBanks()
		{
			controlManager.SendOscControlMessage("/Control/RequestBanks");
		}

		private void RequestPresets()
		{
			controlManager.SendOscControlMessage("/Control/RequestPresets", SelectedBank);
		}

		private void LoadPreset()
		{
			controlManager.SendOscControlMessage("/Control/LoadPreset", SelectedBank, SelectedPreset);
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
			else if (module == "Matrix1")
			{
				Matrix2Visible = false;
				MacrosVisible = false;
				Matrix1Visible = true;
			}
			else if (module == "Matrix2")
			{
				Matrix1Visible = false;
				MacrosVisible = false;
				Matrix2Visible = true;
			}
			else if (module == "Macros")
			{
				Matrix2Visible = false;
				Matrix1Visible = false;
				MacrosVisible = true;
			}
		}

		private bool CheckValidName(string name)
		{
			if (name.Length > 256)
				return false;

			var legalChars = new[] { '-', '_', '(', ')', ' ' };

			// ASCII encoding replaces non-ascii with question marks, so we use UTF8 to see if multi-byte sequences are there
			return name.All(x => (x >= '0' && x <= '9') || (x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z') || legalChars.Contains(x));
		}

		private void SavePreset()
		{
			var presetName = new SavePresetDialog() { Owner = Parent }.ShowDialog("Preset Name");
			if (string.IsNullOrWhiteSpace(presetName))
				return;

			var bankName = SelectedBank;
			if (!CheckValidName(bankName))
				throw new Exception("The selected bank name is invalid");
			if (!CheckValidName(presetName))
				throw new Exception("The selected preset name is invalid");

			controlManager.SendOscControlMessage("/Control/SavePreset", bankName, presetName);
		}
		
		private void DeletePreset()
		{
			
		}
	}
}
