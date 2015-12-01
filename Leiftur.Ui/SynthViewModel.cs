using LowProfile.Core.Ui;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Media;
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
		private readonly Dictionary<int, string> waveforms;

		private Module? activeModule;
		private int? activeParameter;
		private UIElement currentControl;
		private int[] voiceState;

        private bool osc1Visible;
		private bool osc2Visible;
		private bool osc3Visible;
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
		private bool ampEnvPage2Visible;
		private bool filterEnvPage2Visible;
		private bool mainFilterVisible;
		private bool driveVisible;
		private bool mod1Visible;
		private bool mod2Visible;
		private bool mod3Visible;
		private Dictionary<int, string> waveformList;
		private bool presetPanelVisible;
		private Geometry visualGeometry;
		private bool mod1Page2Visible;
		private bool mod2Page2Visible;
		private bool mod3Page2Visible;

		public SynthViewModel(Dictionary<DependencyObject, string> controlDict)
		{
			Application.Current.DispatcherUnhandledException += (s, e) =>
			{
				var ex = e.Exception;
				MessageBox.Show(ex.GetTrace(), "Error", MessageBoxButton.OK, MessageBoxImage.Error);
				e.Handled = true;
			};

			voiceState = new int[24];
			ModuleRoutings = Enum.GetValues(typeof(RoutingStage)).Cast<RoutingStage>().ToDictionary(x => (int)x, x => x.ToString());
			ModSources = Enum.GetValues(typeof(ModSource)).Cast<ModSource>().ToDictionary(x => (int)x, x => x.ToString());
			ModDestinations = Enum.GetValues(typeof(ModDest)).Cast<ModDest>().ToDictionary(x => (int)x, x => x.ToString());
			VoiceModes = new Dictionary<int, string>
			{
				{ (int)VoiceMode.MonoHighest, "Mono - Highest" },
				{ (int)VoiceMode.MonoLowest, "Mono - Lowest" },
				{ (int)VoiceMode.MonoNewest, "Mono - Newest" },
				{ (int)VoiceMode.PolyRoundRobin, "Poly - Round Robin" }
			};

			waveforms = new Dictionary<int, string>();
			presetBanks = new Dictionary<string, List<string>> { { "Default Bank", new List<string>() } };
			controlManager = new ControlManager(this);
			formattedParameters = new Dictionary<int, string>();
			announcerCaption = "";
			announcerValue = "";

			controls = new Dictionary<int, DependencyObject>();
			RegisterControls(controlDict);

			ShowAmpEnvPage2 = new DelegateCommand(_ => AmpEnvPage2Visible = !AmpEnvPage2Visible);
			ShowFilterEnvPage2 = new DelegateCommand(_ => FilterEnvPage2Visible = !FilterEnvPage2Visible);
			ShowMod1EnvPage2 = new DelegateCommand(_ => Mod1Page2Visible = !Mod1Page2Visible);
			ShowMod2EnvPage2 = new DelegateCommand(_ => Mod2Page2Visible = !Mod2Page2Visible);
			ShowMod3EnvPage2 = new DelegateCommand(_ => Mod3Page2Visible = !Mod3Page2Visible);
			SetBankCommand = new DelegateCommand(x => SelectedBank = x.ToString(), () => true);
			SetPresetCommand = new DelegateCommand(x => SelectedPreset = x.ToString(), () => true);
			SavePresetCommand = new DelegateCommand(x => SavePreset(), () => true);
			DeletePresetCommand = new DelegateCommand(x => DeletePreset(), () => true);
			Osc1Visible = true;
			MainFilterVisible = true;
			Mod1Visible = true;
			ChorusVisible = true;
			Matrix1Visible = true;
			PresetPanelVisible = true;

			SelectedBank = "Unknown Bank";
			SelectedPreset = "Unknown Preset";
		}

		#region Properties

		public DelegateCommand ShowAmpEnvPage2 { get; private set; }
		public DelegateCommand ShowFilterEnvPage2 { get; private set; }
		public DelegateCommand ShowMod1EnvPage2 { get; private set; }
		public DelegateCommand ShowMod2EnvPage2 { get; private set; }
		public DelegateCommand ShowMod3EnvPage2 { get; private set; }

		public DelegateCommand SetBankCommand { get; private set; }
		public DelegateCommand SetPresetCommand { get; private set; }
		public DelegateCommand SavePresetCommand { get; private set; }
		public DelegateCommand DeletePresetCommand { get; private set; }

		public Window Parent { get; set; }

		public string[] Banks => presetBanks.Keys.ToArray();

		public string[] Presets => (presetBanks.GetValueOrDefault(selectedBank) ?? new List<string>()).ToArray();

		public Dictionary<int, string> WaveformList
		{
			get { return waveformList; }
			set { waveformList = value; NotifyPropertyChanged(); }
		}

		public Dictionary<int, string> ModuleRoutings { get; set; }
		public Dictionary<int, string> VoiceModes { get; set; }
		public Dictionary<int, string> ModSources { get; set; }
		public Dictionary<int, string> ModDestinations { get; set; }

		public int[] VoiceState
		{
			get { return voiceState.Select(x => x).ToArray(); }
		}

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

		public bool PresetPanelVisible
		{
			get { return presetPanelVisible; }
			set
			{
				presetPanelVisible = value;
				NotifyPropertyChanged();
				NotifyPropertyChanged(nameof(VisualPanelVisible));
			}
		}

		public bool VisualPanelVisible => !PresetPanelVisible;

		public Geometry VisualGeometry
		{
			get { return visualGeometry; }
			set { visualGeometry = value; NotifyPropertyChanged(); }
		}

		public bool Osc1Visible
		{
			get { return osc1Visible; }
			set
			{
				if (osc1Visible || !value)
					return;

				osc2Visible = false;
				osc3Visible = false;
				osc1Visible = true;
				NotifyPropertyChanged(nameof(Osc1Visible));
				NotifyPropertyChanged(nameof(Osc2Visible));
				NotifyPropertyChanged(nameof(Osc3Visible));
			}
		}

		public bool Osc2Visible
		{
			get { return osc2Visible; }
			set
			{
				if (osc2Visible || !value)
					return;

				osc1Visible = false;
				osc3Visible = false;
				osc2Visible = true;
				NotifyPropertyChanged(nameof(Osc1Visible));
				NotifyPropertyChanged(nameof(Osc2Visible));
				NotifyPropertyChanged(nameof(Osc3Visible));
			}
		}

		public bool Osc3Visible
		{
			get { return osc3Visible; }
			set
			{
				if (osc3Visible || !value)
					return;

				osc1Visible = false;
				osc2Visible = false;
				osc3Visible = true;
				NotifyPropertyChanged(nameof(Osc1Visible));
				NotifyPropertyChanged(nameof(Osc2Visible));
				NotifyPropertyChanged(nameof(Osc3Visible));
			}
		}

		public bool MainFilterVisible
		{
			get { return mainFilterVisible; }
			set
			{
				if (MainFilterVisible || !value)
					return;

				driveVisible = false;
				mainFilterVisible = true;
				NotifyPropertyChanged(nameof(DriveVisible));
				NotifyPropertyChanged(nameof(MainFilterVisible));
			}
		}

		public bool DriveVisible
		{
			get { return driveVisible; }
			set
			{
				if (DriveVisible || !value)
					return;
				
				mainFilterVisible = false;
				driveVisible = true;
				NotifyPropertyChanged(nameof(DriveVisible));
				NotifyPropertyChanged(nameof(MainFilterVisible));
			}
		}

		public bool Mod1Visible
		{
			get { return mod1Visible; }
			set
			{
				if (mod1Visible || !value)
					return;

				mod2Visible = false;
				mod3Visible = false;
				arpeggiatorVisible = false;
				mod1Visible = true;
				NotifyPropertyChanged(nameof(ArpeggiatorVisible));
				NotifyPropertyChanged(nameof(Mod1Visible));
				NotifyPropertyChanged(nameof(Mod2Visible));
				NotifyPropertyChanged(nameof(Mod3Visible));
			}
		}

		public bool Mod2Visible
		{
			get { return mod2Visible; }
			set
			{
				if (mod2Visible || !value)
					return;

				mod1Visible = false;
				mod3Visible = false;
				arpeggiatorVisible = false;
				mod2Visible = true;
				NotifyPropertyChanged(nameof(ArpeggiatorVisible));
				NotifyPropertyChanged(nameof(Mod1Visible));
				NotifyPropertyChanged(nameof(Mod2Visible));
				NotifyPropertyChanged(nameof(Mod3Visible));
			}
		}

		public bool Mod3Visible
		{
			get { return mod3Visible; }
			set
			{
				if (mod3Visible || !value)
					return;

				mod1Visible = false;
				mod2Visible = false;
				arpeggiatorVisible = false;
				mod3Visible = true;
				NotifyPropertyChanged(nameof(ArpeggiatorVisible));
				NotifyPropertyChanged(nameof(Mod1Visible));
				NotifyPropertyChanged(nameof(Mod2Visible));
				NotifyPropertyChanged(nameof(Mod3Visible));
			}
		}

		public bool ArpeggiatorVisible
		{
			get { return arpeggiatorVisible; }
			set
			{
				if (arpeggiatorVisible || !value)
					return;

				mod1Visible = false;
				mod2Visible = false;
				mod3Visible = false;
				arpeggiatorVisible = true;
				NotifyPropertyChanged(nameof(ArpeggiatorVisible));
				NotifyPropertyChanged(nameof(Mod1Visible));
				NotifyPropertyChanged(nameof(Mod2Visible));
				NotifyPropertyChanged(nameof(Mod3Visible));
			}
		}

		public bool DelayVisible
		{
			get { return delayVisible; }
			set
			{
				if (delayVisible || !value)
					return;

				chorusVisible = false;
				delayVisible = true;
				NotifyPropertyChanged(nameof(ChorusVisible));
				NotifyPropertyChanged(nameof(DelayVisible));
			}
		}

		public bool ChorusVisible
		{
			get { return chorusVisible; }
			set
			{
				if (chorusVisible || !value)
					return;

				delayVisible = false;
				chorusVisible = true;
				NotifyPropertyChanged(nameof(ChorusVisible));
				NotifyPropertyChanged(nameof(DelayVisible));
			}
		}

		public bool Matrix1Visible
		{
			get { return matrix1Visible; }
			set
			{
				if (matrix1Visible || !value)
					return;

				matrix2Visible = false;
				macrosVisible = false;
				matrix1Visible = true;
				NotifyPropertyChanged(nameof(MacrosVisible));
				NotifyPropertyChanged(nameof(Matrix1Visible));
				NotifyPropertyChanged(nameof(Matrix2Visible));
			}
		}

		public bool Matrix2Visible
		{
			get { return matrix2Visible; }
			set
			{
				if (matrix2Visible || !value)
					return;

				matrix1Visible = false;
				macrosVisible = false;
				matrix2Visible = true;
				NotifyPropertyChanged(nameof(MacrosVisible));
				NotifyPropertyChanged(nameof(Matrix1Visible));
				NotifyPropertyChanged(nameof(Matrix2Visible));
			}
		}

		public bool MacrosVisible
		{
			get { return macrosVisible; }
			set
			{
				if (macrosVisible || !value)
					return;

				matrix1Visible = false;
				matrix2Visible = false;
				macrosVisible = true;
				NotifyPropertyChanged(nameof(MacrosVisible));
				NotifyPropertyChanged(nameof(Matrix1Visible));
				NotifyPropertyChanged(nameof(Matrix2Visible));
			}
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

		public bool Mod1Page2Visible
		{
			get { return mod1Page2Visible; }
			set { mod1Page2Visible = value; NotifyPropertyChanged(); }
		}

		public bool Mod2Page2Visible
		{
			get { return mod2Page2Visible; }
			set { mod2Page2Visible = value; NotifyPropertyChanged(); }
		}

		public bool Mod3Page2Visible
		{
			get { return mod3Page2Visible; }
			set { mod3Page2Visible = value; NotifyPropertyChanged(); }
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
			RequestWaveforms();
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
				PrepareVisualRequest(module);
			}
			else if (msg.Address == "/Control/Waveforms")
			{
				for (int i = 0; i < msg.Arguments.Length - 1; i+=2)
				{
					var id = (int)msg.Arguments[i];
					var specifier = ((string)msg.Arguments[i + 1]).Replace("/Wavetables/", "");
					waveforms[id] = specifier;
				}

				WaveformList = waveforms.ToDictionary(x => x.Key, x => x.Value);
			}
			else if (msg.Address == "/Control/Banks")
			{
				if (msg.Arguments.Length != 1 || !(msg.Arguments[0] is string))
					return;

				var bankString = (string)msg.Arguments.First();
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

				var bankName = (string)msg.Arguments.First();
				var presets = ((string)msg.Arguments.Last()).Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries);

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
			else if (msg.Address.StartsWith("/Control/Visual"))
			{
				UpdateVisual((byte[])msg.Arguments[0]);
			}
			else if (msg.Address == "/Control/VoiceState")
			{
				SetVoiceState((byte[])msg.Arguments[0]);
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
					if (control is LightKnob)
					{
						((LightKnob)control).Value = value;
					}
					else if (control is ListBox)
					{
						var lb = (ListBox)control;
						var source = lb.ItemsSource as Dictionary<int, string>;
						if (source == null) return;

						var dictKey = (int)(value + 0.0001);
                        var selected = source.ContainsKey(dictKey) ? source.Single(x => x.Key == dictKey) : source.First();
						lb.SelectedItem = selected;
					}
					else if (control is ToggleButton)
					{
						((ToggleButton)control).IsChecked = value > 0.5;
					}
					else if (control is Spinner)
					{
						((Spinner)control).Value = value;
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

		private void RequestWaveforms()
		{
			waveforms.Clear();
			controlManager.SendOscControlMessage("/Control/RequestWaveforms");
		}

		private void RequestPresets()
		{
			controlManager.SendOscControlMessage("/Control/RequestPresets", SelectedBank);
		}

		private void RequestVisual(Module module)
		{
			controlManager.SendOscControlMessage("/Control/RequestVisual/" + module);
		}

		private void LoadPreset()
		{
			controlManager.SendOscControlMessage("/Control/LoadPreset", SelectedBank, SelectedPreset);
		}

		private void RegisterControls(Dictionary<DependencyObject, string> controlDict)
		{
			foreach (var control in controlDict.Keys)
			{
				if (!(control is UIElement))
					continue;
				
				UIElement uiElem = control as UIElement;
				uiElem.MouseEnter += (s, e) => SetActiveControl(s as UIElement, true);
				uiElem.MouseLeave += (s, e) => SetActiveControl(s as UIElement, false);
				
				var address = OscAddress.GetAddress(control);
				var key = Parameters.Pack(address);
				controls[key] = control;

				var lightKnob = control as LightKnob;
				if (lightKnob != null)
				{
					DependencyPropertyDescriptor.FromProperty(LightKnob.ValueProperty, typeof(LightKnob))
						.AddValueChanged(lightKnob, (s, e) => SendValue(address, lightKnob.Value));
				}

				var listbox = control as ListBox;
				if (listbox != null)
				{
					DependencyPropertyDescriptor.FromProperty(Selector.SelectedItemProperty, typeof(ListBox))
						.AddValueChanged(listbox, (s, e) => SendValue(address, ((KeyValuePair<int, string>)listbox.SelectedItem).Key));
				}

				var toggle = control as ToggleButton;
				if (toggle != null)
				{
					DependencyPropertyDescriptor.FromProperty(ToggleButton.IsCheckedProperty, typeof(ToggleButton))
						.AddValueChanged(toggle, (s, e) => SendValue(address, toggle.IsChecked.GetValueOrDefault() ? 1 : 0));
				}

				var spinner = control as Spinner;
				if (spinner != null)
				{
					DependencyPropertyDescriptor.FromProperty(Spinner.ValueProperty, typeof(Spinner))
						.AddValueChanged(spinner, (s, e) => SendValue(address, spinner.Value));
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

				PrepareVisualRequest(tuple.Item1);
			}
			else if (currentControl.Equals(uiElement))
			{
				currentControl = null;
				ClearActiveParameter();
				PresetPanelVisible = true;
			}
		}

		private void PrepareVisualRequest(Module module)
		{
			if (new[] { Module.EnvAmp, Module.EnvFilter, Module.Osc1, Module.Osc2, Module.Osc3 }.Contains(module))
			{
				PresetPanelVisible = false;
				RequestVisual(module);
			}
			else
			{
				PresetPanelVisible = true;
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

		private void UpdateVisual(byte[] data)
		{
			var width = 400;
			var height = 80;

			var sb = new StringBuilder();
			for (int i = 0; i < data.Length; i++)
			{
				var x = i / (double)data.Length * width;
				var y = height - data[i] / 255.5 * height;

				if (i == 0)
				{
					sb.Append($"M{x},{y} ");
				}
				else
					sb.Append($"L{x},{y} ");
			}

			VisualGeometry = Geometry.Parse(sb.ToString());
		}

		private void SetVoiceState(byte[] state)
		{
			for (int i = 0; i < state.Length; i++)
			{
				if (i < voiceState.Length)
					voiceState[i] = state[i];
			}

			NotifyPropertyChanged(nameof(VoiceState));
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
