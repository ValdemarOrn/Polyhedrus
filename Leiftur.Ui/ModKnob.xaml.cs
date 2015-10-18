using System;
using System.ComponentModel;
using System.Globalization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace Leiftur.Ui
{
	/// <summary>
	/// Interaction logic for ModKnob.xaml
	/// </summary>
	public partial class ModKnob : UserControl
	{
		static internal DependencyProperty ValueProperty = DependencyProperty.Register("Value", typeof(double), typeof(ModKnob),
				new FrameworkPropertyMetadata(0.0, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static internal DependencyProperty ModValueProperty = DependencyProperty.Register("ModValue", typeof(double), typeof(ModKnob),
				new FrameworkPropertyMetadata(0.0, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static internal DependencyProperty IndicatorLineDataProperty = DependencyProperty.Register("IndicatorLineData", typeof(Geometry), typeof(ModKnob),
				new FrameworkPropertyMetadata(Geometry.Parse("M01,40 L20,40"), FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static internal DependencyProperty ModLineDataProperty = DependencyProperty.Register("ModLineData", typeof(Geometry), typeof(ModKnob),
				new FrameworkPropertyMetadata(Geometry.Parse(""), FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		static internal DependencyProperty DeltaProperty = DependencyProperty.Register("Delta", typeof(double), typeof(ModKnob),
				new FrameworkPropertyMetadata(0.0035, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		public ModKnob()
		{
			InitializeComponent();

			DependencyPropertyDescriptor.FromProperty(ValueProperty, this.GetType())
				.AddValueChanged(this, (s, e) => { Recalculate(); });

			DependencyPropertyDescriptor.FromProperty(ModValueProperty, this.GetType())
				.AddValueChanged(this, (s, e) => { Recalculate(); });
		}

		public double Value
		{
			get { return (double)base.GetValue(ValueProperty); }
			set { SetValue(ValueProperty, value); }
		}

		public double ModValue
		{
			get { return (double)base.GetValue(ModValueProperty); }
			set { SetValue(ModValueProperty, value); }
		}

		public Geometry IndicatorLineData
		{
			get { return (Geometry)base.GetValue(IndicatorLineDataProperty); }
			set { SetValue(IndicatorLineDataProperty, value); }
		}

		public Geometry ModLineData
		{
			get { return (Geometry)base.GetValue(ModLineDataProperty); }
			set { SetValue(ModLineDataProperty, value); }
		}

		public double Delta
		{
			get { return (double)base.GetValue(DeltaProperty); }
			set { SetValue(DeltaProperty, value); }
		}

		void Recalculate()
		{
			// indicator

			var radius = 40;
			var angle = 225.0 - Value * 270.0;
			var dx = Math.Cos(ToRad(angle));
			var dy = Math.Sin(ToRad(angle));

			var x0 = radius + dx * (radius - 1);
			var y0 = radius - dy * (radius - 1);

			var x1 = radius + dx * (radius - 1 - 20);
			var y1 = radius - dy * (radius - 1 - 20);

			string geometry = String.Format(CultureInfo.InvariantCulture, @"M{0},{1} L{2},{3}", x0, y0, x1, y1);
			IndicatorLineData = Geometry.Parse(geometry);

			// mod

			radius = 60;
			x0 = radius + dx * (radius - 5);
			y0 = radius - dy * (radius - 5);

			var diff = ModValue;
			if (Value + diff < 0)
				diff = - Value;
			else if (Value + diff > 1)
				diff = 1 - Value;

			var dAngle = diff * 270.0;
			angle = angle - dAngle;
			dx = Math.Cos(ToRad(angle));
			dy = Math.Sin(ToRad(angle));

			x1 = radius + dx * (radius - 5);
			y1 = radius - dy * (radius - 5);

			var isLarge = (dAngle >= 180 || dAngle <= -180) ? "1" : "0";
			var isPositive = (dAngle > 0) ? "1" : "0";

			geometry = String.Format(CultureInfo.InvariantCulture, @"M{0},{1} A 55,55 {2} {3} {4} {5},{6}", x0, y0, dAngle, isLarge, isPositive, x1, y1);
			ModLineData = Geometry.Parse(geometry);
		}

		double ToRad(double degrees)
		{
			//return degrees / 360.0 * 2 * Math.PI;
			return degrees * 0.00277777777777777777 * 2 * Math.PI;
		}

		bool Selected;
		Point MousePos;

		private void OnMouseDown(object sender, MouseButtonEventArgs e)
		{
			if (Mouse.LeftButton == MouseButtonState.Released)
				return;

			Selected = true;
			Mouse.Capture(this);
			MousePos = e.GetPosition(this);
		}

		private void OnMouseUp(object sender, MouseButtonEventArgs e)
		{
			Selected = false;
			Mouse.Capture(null);
			MousePos = e.GetPosition(this);
		}

		private void OnMouseMove(object sender, MouseEventArgs e)
		{
			if (Mouse.LeftButton == MouseButtonState.Released)
			{
				Selected = false;
				Mouse.Capture(null);
				MousePos = e.GetPosition(this);
				return;
			}

			if (!Selected)
				return;

			var oldPos = MousePos;
			MousePos = e.GetPosition(this);

			var dx = oldPos.Y - MousePos.Y;

			if (Math.Abs(dx) < 0.5)
				return;

			// fine tune with shift
			if (Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift))
				dx *= 0.2;

			if (Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl))
				ChangeModValue(dx);
			else
				ChangeValue(dx);
		}

		private void ChangeValue(double dx)
		{
			var oldVal = Value;
			var val = oldVal + Delta * dx;

			if (val < 0.0)
				val = 0.0;
			else if (val > 1.0)
				val = 1.0;

			if (val != oldVal)
				Value = val;
		}

		private void ChangeModValue(double dx)
		{
			var oldVal = ModValue;
			var val = oldVal + Delta * dx;

			if (val < -1.0)
				val = -1.0;
			else if (val > 1.0)
				val = 1.0;

			if (val != oldVal)
				ModValue = val;
		}
		
	}
}
