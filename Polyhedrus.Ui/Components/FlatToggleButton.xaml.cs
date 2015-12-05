using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Polyhedrus.Ui.Components
{
	/// <summary>
	/// Interaction logic for FlatToggleButton.xaml
	/// </summary>
	public partial class FlatToggleButton : ToggleButton
	{
		static new internal DependencyProperty BorderBrushProperty = DependencyProperty.Register("BorderBrush", typeof(Brush), typeof(FlatToggleButton),
			new FrameworkPropertyMetadata(new SolidColorBrush(Colors.Black), FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static new internal DependencyProperty BackgroundProperty = DependencyProperty.Register("Background", typeof(Brush), typeof(FlatToggleButton),
			new FrameworkPropertyMetadata(new SolidColorBrush(Colors.White), FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static internal DependencyProperty TextProperty = DependencyProperty.Register("Text", typeof(string), typeof(FlatToggleButton),
				new FrameworkPropertyMetadata("Off", FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		private string textOn;
		private string textOff;

		public FlatToggleButton()
		{
			InitializeComponent();

			DependencyPropertyDescriptor prop = DependencyPropertyDescriptor.FromProperty(IsCheckedProperty, this.GetType());
			prop.AddValueChanged(this, (x, y) => SetValue(TextProperty, IsChecked.GetValueOrDefault() ? TextOn ?? "On" : TextOff ?? "Off"));
		}

		public new Brush Background
		{
			get { return (Brush)base.GetValue(BorderBrushProperty); }
			set { SetValue(BorderBrushProperty, value); }
		}

		public new Brush BorderBrush
		{
			get { return (Brush)base.GetValue(BorderBrushProperty); }
			set { SetValue(BorderBrushProperty, value); }
		}

		public string Text
		{
			get { return (string)base.GetValue(TextProperty); }
		}

		public string TextOn
		{
			get
			{
				return textOn;
			}
			set
			{
				textOn = value;
				if (IsChecked.GetValueOrDefault())
					SetValue(TextProperty, value);
			}
		}

		public string TextOff
		{
			get
			{
				return textOff;
			}
			set
			{
				textOff = value;
				if (!IsChecked.GetValueOrDefault())
					SetValue(TextProperty, value);
			}
		}
	}
}
