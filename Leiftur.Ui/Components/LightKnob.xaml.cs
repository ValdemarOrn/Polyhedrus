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
	/// Interaction logic for LightKnob.xaml
	/// </summary>
	public partial class LightKnob : UserControl
	{
		static internal DependencyProperty ValueProperty = DependencyProperty.Register("Value", typeof(double), typeof(LightKnob),
				new FrameworkPropertyMetadata(0.0, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static internal DependencyProperty StepsProperty = DependencyProperty.Register("Steps", typeof(int), typeof(LightKnob),
				new FrameworkPropertyMetadata(1000, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static internal DependencyProperty MinProperty = DependencyProperty.Register("Min", typeof(double), typeof(LightKnob),
				new FrameworkPropertyMetadata(0.0, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static internal DependencyProperty MaxProperty = DependencyProperty.Register("Max", typeof(double), typeof(LightKnob),
				new FrameworkPropertyMetadata(1.0, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static internal DependencyProperty DefaultValueProperty = DependencyProperty.Register("DefaultValue", typeof(double?), typeof(LightKnob),
				new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static internal DependencyProperty IndicatorAngleProperty = DependencyProperty.Register("IndicatorAngle", typeof(double), typeof(LightKnob),
				new FrameworkPropertyMetadata(-45.0, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static internal DependencyProperty DeltaProperty = DependencyProperty.Register("Delta", typeof(double), typeof(LightKnob),
				new FrameworkPropertyMetadata(0.0035, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		bool selected;
		Point mousePos;
		private bool disableUpdates;

		public LightKnob()
		{
			InitializeComponent();

			DependencyPropertyDescriptor.FromProperty(ValueProperty, this.GetType())
				.AddValueChanged(this, (s, e) => { SetInnerValue(); Recalculate(); });

			DependencyPropertyDescriptor.FromProperty(DefaultValueProperty, this.GetType())
				.AddValueChanged(this, (s, e) =>
				{
					if (DefaultValue.HasValue)
					{
						Value = DefaultValue.Value;
						SetInnerValue();
						Recalculate();
					}
				});

			SetInnerValue();
			Recalculate();

			MouseDoubleClick += MouseDoubleClickHandler;
			MouseDown += MouseDownHandler;
			MouseUp += MouseUpHandler;
			MouseMove += MouseMoveHandler;
		}

		private double QuantizedValue
		{
			get
			{
				var intVal = (int)(innerValue * (Steps - 0.00001));
				return intVal / (double)(Steps - 1);
			}
		}

		private double innerValue;

		public double Value
		{
			get { return (double)base.GetValue(ValueProperty); }
			set { SetValue(ValueProperty, value); }
		}

		public int Steps
		{
			get { return (int)base.GetValue(StepsProperty); }
			set { SetValue(StepsProperty, value); }
		}

		public double Min
		{
			get { return (double)base.GetValue(MinProperty); }
			set { SetValue(MinProperty, value); }
		}

		public double Max
		{
			get { return (double)base.GetValue(MaxProperty); }
			set { SetValue(MaxProperty, value); }
		}

		public double? DefaultValue
		{
			get { return (double?)base.GetValue(DefaultValueProperty); }
			set { SetValue(DefaultValueProperty, value); }
		}

		public double IndicatorAngle
		{
			get { return (double)base.GetValue(IndicatorAngleProperty); }
			set { SetValue(IndicatorAngleProperty, value); }
		}

		public double Delta
		{
			get { return (double)base.GetValue(DeltaProperty); }
			set { SetValue(DeltaProperty, value); }
		}

		private void SetInnerValue()
		{
			if (disableUpdates)
				return;

			innerValue = (Value - Min) / (Max - Min);
		}

		void Recalculate()
		{
			var stroke = 8.0;
			var w = 120.0 - stroke;
			var radius = w / 2;
			var defaultInnerValue = ((DefaultValue ?? Min) - Min) / (Max - Min);

			// indicator
			IndicatorAngle = -45 + QuantizedValue * 270.0;

			var angleRad = (225.0 - QuantizedValue * 270) / 360 * 2 * Math.PI;
			var defaultRad = (225.0 - defaultInnerValue * 270) / 360 * 2 * Math.PI;
			var minRad = (225.0) / 360 * 2 * Math.PI;
			var maxRad = (225.0 - 270) / 360 * 2 * Math.PI;

			var dxMin = Math.Cos(minRad) * w * 0.5 + stroke * 0.5 + w * 0.5;
			var dyMin = -Math.Sin(minRad) * w * 0.5 + stroke * 0.5 + w * 0.5;
			var dxMax = Math.Cos(maxRad) * w * 0.5 + stroke * 0.5 + w * 0.5;
			var dyMax = -Math.Sin(maxRad) * w * 0.5 + stroke * 0.5 + w * 0.5;

			var dxDef = Math.Cos(defaultRad) * w * 0.5 + stroke * 0.5 + w * 0.5;
			var dyDef = -Math.Sin(defaultRad) * w * 0.5 + stroke * 0.5 + w * 0.5;
			var dx = Math.Cos(angleRad) * w * 0.5 + stroke * 0.5 + w * 0.5;
			var dy = -Math.Sin(angleRad) * w * 0.5 + stroke * 0.5 + w * 0.5;
			var isLarge = Math.Abs(angleRad - defaultRad) >= Math.PI ? 1 : 0;
			var isNeg = angleRad > defaultRad ? 0 : 1;

            ModLine1.Data = Geometry.Parse($"M{dxDef},{dyDef} A {radius},{radius} 90 {isLarge} {isNeg} {dx},{dy}");
			ModLine2.Data = Geometry.Parse($"M{dxMin},{dyMin} A {radius},{radius} 90 1 1 {dxMax},{dyMax}");
		}
		
		private void MouseDoubleClickHandler(object sender, MouseButtonEventArgs e)
		{
			Value = DefaultValue ?? Min;
		}

		private void MouseDownHandler(object sender, MouseButtonEventArgs e)
		{
			if (Mouse.LeftButton == MouseButtonState.Released)
				return;

			selected = true;
			Mouse.Capture(this);
			mousePos = e.GetPosition(this);
		}

		private void MouseUpHandler(object sender, MouseButtonEventArgs e)
		{
			selected = false;
			Mouse.Capture(null);
			mousePos = e.GetPosition(this);
		}

		private void MouseMoveHandler(object sender, MouseEventArgs e)
		{
			if (Mouse.LeftButton == MouseButtonState.Released)
			{
				selected = false;
				Mouse.Capture(null);
				mousePos = e.GetPosition(this);
				return;
			}

			if (!selected)
				return;

			var oldPos = mousePos;
			mousePos = e.GetPosition(this);

			var dx = oldPos.Y - mousePos.Y;

			if (Math.Abs(dx) < 0.5)
				return;

			// fine tune with shift
			if (Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift))
				dx *= 0.2;

			ChangeValue(dx);
		}

		private void ChangeValue(double dx)
		{
			var oldVal = innerValue;
			var val = oldVal + Delta * dx;

			if (val < 0.0)
				val = 0.0;
			else if (val > 1.0)
				val = 1.0;

			if (val != oldVal)
			{
				try
				{
					disableUpdates = true;
					innerValue = val;
					Value = Min + QuantizedValue * (Max - Min);
				}
				finally
				{
					disableUpdates = false;
				}
			}
		}
	}
}
