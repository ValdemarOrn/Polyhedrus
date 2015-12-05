using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Polyhedrus.Ui.Components
{
	
	public partial class VoiceIndicator : UserControl
	{
		static internal DependencyProperty VoiceIsActiveProperty = DependencyProperty.Register("VoiceIsActive", typeof(bool), typeof(VoiceIndicator),
				new FrameworkPropertyMetadata(false, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static internal DependencyProperty VoiceIsDecayingProperty = DependencyProperty.Register("VoiceIsDecaying", typeof(bool), typeof(VoiceIndicator),
				new FrameworkPropertyMetadata(false, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static internal DependencyProperty StateProperty = DependencyProperty.Register("State", typeof(int), typeof(VoiceIndicator),
				new FrameworkPropertyMetadata(0, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		public VoiceIndicator()
		{
			InitializeComponent();

			DependencyPropertyDescriptor.FromProperty(StateProperty, this.GetType())
				.AddValueChanged(this, (s, e) =>
				{
					VoiceIsActive = State == 2;
					VoiceIsDecaying = State == 1;
				});
		}

		public bool VoiceIsActive
		{
			get { return (bool)GetValue(VoiceIsActiveProperty); }
			set { SetValue(VoiceIsActiveProperty, value); }
		}

		public bool VoiceIsDecaying
		{
			get { return (bool)GetValue(VoiceIsDecayingProperty); }
			set { SetValue(VoiceIsDecayingProperty, value); }
		}

		public int State
		{
			get { return (int)GetValue(StateProperty); }
			set { SetValue(StateProperty, value); }
		}
	}
}
