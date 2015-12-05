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

namespace Polyhedrus.Ui.Components
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
			new FrameworkPropertyMetadata("0.00", FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static new public DependencyProperty BorderBrushProperty = DependencyProperty.Register("BorderBrush", typeof(Brush), typeof(Spinner),
				new FrameworkPropertyMetadata(new SolidColorBrush(Colors.Black), FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static public DependencyProperty FillProperty = DependencyProperty.Register("Fill", typeof(Brush), typeof(Spinner),
				new FrameworkPropertyMetadata(new SolidColorBrush(Colors.White), FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		private double innerValue;

		public Spinner()
		{
			Delta = 0.01;
			Quantize = 0.01;
			Min = 0;
			Max = 1;

			InitializeComponent();

			// add change listeners to props
			DependencyPropertyDescriptor prop = DependencyPropertyDescriptor.FromProperty(ValueProperty, GetType());
			prop.AddValueChanged(this, (x, y) =>
			{
				innerValue = Value;
				SetValue(ValueTextProperty, FormattedValue);
			});

			prop = DependencyPropertyDescriptor.FromProperty(FormatterProperty, GetType());
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
					return Value.ToString("0.000", CultureInfo.InvariantCulture);
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
		
		public double Delta { get; set; }
		public double Quantize { get; set; }
		public double Min { get; set; }
		public double Max { get; set; }
		public double Default { get; set; }

		private bool isSelected;
		private Point? lastMousePos;

		private void OnMouseDown(object sender, MouseButtonEventArgs e)
		{
			if (Mouse.LeftButton == MouseButtonState.Released)
				return;

			isSelected = true;
			Mouse.Capture(this);
			lastMousePos = e.GetPosition(this);
		}

		private void OnMouseUp(object sender, MouseButtonEventArgs e)
		{
			isSelected = false;
			Mouse.Capture(null);
			lastMousePos = null;
		}

		private void OnMouseMove(object sender, MouseEventArgs e)
		{
			if (Mouse.LeftButton == MouseButtonState.Released)
			{
				isSelected = false;
				Mouse.Capture(null);
				lastMousePos = null;
				return;
			}

			if (!isSelected || !lastMousePos.HasValue)
				return;

			var currentPos = e.GetPosition(this);
			double dx = (int)(lastMousePos.Value.Y - currentPos.Y);
			lastMousePos = currentPos;

            if (Math.Abs(dx) < 1.0)
				return;

			if (Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift))
				dx *= 0.1;

			var val = innerValue + Delta * dx;

			if (val < Min)
				val = Min;
			else if (val > Max)
				val = Max;

			innerValue = val;
			var valQuantized = Math.Round(val / Quantize) * Quantize;
			if (valQuantized < Min)
				valQuantized = Min;
			else if (valQuantized > Max)
				valQuantized = Max;

			if (valQuantized != Value)
			{
				Value = valQuantized;
			}
		}

		private void OnMouseDoubleClick(object sender, MouseButtonEventArgs e)
		{
			Value = Default;
			
		}
	}
}
