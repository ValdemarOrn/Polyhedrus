using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
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

namespace Leiftur.Ui.Components
{
	/// <summary>
	/// Interaction logic for Spinnder.xaml
	/// </summary>
	public partial class Spinner : UserControl
	{
		static public DependencyProperty ValueProperty = DependencyProperty.Register("Value", typeof(double), typeof(Spinner),
			new FrameworkPropertyMetadata(0.0, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static public DependencyProperty ValueTextProperty = DependencyProperty.Register("ValueText", typeof(string), typeof(Spinner));

		static public DependencyProperty FormatterProperty = DependencyProperty.Register("Formatter", typeof(string), typeof(Spinner),
			new FrameworkPropertyMetadata("{0.00}", FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static new public DependencyProperty BorderBrushProperty = DependencyProperty.Register("BorderBrush", typeof(Brush), typeof(Spinner),
				new FrameworkPropertyMetadata(new SolidColorBrush(Colors.Black), FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static public DependencyProperty FillProperty = DependencyProperty.Register("Fill", typeof(Brush), typeof(Spinner),
				new FrameworkPropertyMetadata(new SolidColorBrush(Colors.White), FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		public Spinner()
		{
			Delta = 0.005;
			Min = 0;
			Max = 1;
			InitializeComponent();

			// add change listeners to props
			DependencyPropertyDescriptor prop = DependencyPropertyDescriptor.FromProperty(ValueProperty, this.GetType());
			prop.AddValueChanged(this, (x, y) => SetValue(ValueTextProperty, FormattedValue));

			prop = DependencyPropertyDescriptor.FromProperty(FormatterProperty, this.GetType());
			prop.AddValueChanged(this, (x, y) => SetValue(ValueTextProperty, FormattedValue));
		}

		public double Value
		{
			get { return (double)GetValue(ValueProperty); }
			set { SetValue(ValueProperty, value); }
		}

		public string ValueText
		{
			get { return (string)GetValue(ValueTextProperty); }
			set { SetValue(ValueTextProperty, value); }
		}

		private string FormattedValue
		{
			get
			{
				if (Formatter == null)
					return Value.ToString("{0.000}", CultureInfo.InvariantCulture);
				else
					return Value.ToString(Formatter, CultureInfo.InvariantCulture);
			}
		}

		public string Formatter
		{
			get { return (string)base.GetValue(FormatterProperty); }
			set
			{
				SetValue(FormatterProperty, value);
				ValueText = FormattedValue;
			}
		}

		public new Brush BorderBrush
		{
			get { return (Brush)GetValue(BorderBrushProperty); }
			set { SetValue(BorderBrushProperty, value); }
		}

		public Brush Fill
		{
			get { return (Brush)GetValue(FillProperty); }
			set { SetValue(FillProperty, value); }
		}

		public int Sensitivity { get; set; }
		public double Delta { get; set; }
		public double Min { get; set; }
		public double Max { get; set; }
		public double Default { get; set; }

		private bool isSelected;
		private Point? mousePosStart;
		private double valueStart;

		private void OnMouseDown(object sender, MouseButtonEventArgs e)
		{
			if (Mouse.LeftButton == MouseButtonState.Released)
				return;

			isSelected = true;
			Mouse.Capture(this);
			mousePosStart = e.GetPosition(this);
			valueStart = Value;
		}

		private void OnMouseUp(object sender, MouseButtonEventArgs e)
		{
			isSelected = false;
			Mouse.Capture(null);
			mousePosStart = null;
		}

		private void OnMouseMove(object sender, MouseEventArgs e)
		{
			if (Mouse.LeftButton == MouseButtonState.Released)
			{
				isSelected = false;
				Mouse.Capture(null);
				mousePosStart = null;
				return;
			}

			if (!isSelected || !mousePosStart.HasValue)
				return;

			var curentPos = e.GetPosition(this);
			double dx = (int)((mousePosStart.Value.Y - curentPos.Y) / Sensitivity);

			if (Math.Abs(dx) < 1.0)
				return;

			if (Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift))
				dx *= 0.1;

			var val = valueStart + Delta * dx;

			if (val < Min)
				val = Min;
			else if (val > Max)
				val = Max;

			if (val != valueStart)
			{
				Value = val;
				valueStart = val;
				mousePosStart = curentPos;
			}
		}

		private void OnMouseDoubleClick(object sender, MouseButtonEventArgs e)
		{
			Value = Default;
		}
	}
}
