using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace Polyhedrus.Ui
{
	class OscAddress : FrameworkElement
	{
		public static string GetAddress(DependencyObject obj)
		{
			return (string)obj.GetValue(ParameterProperty);
		}

		public static void SetAddress(DependencyObject obj, string value)
		{
			obj.SetValue(ParameterProperty, value);
		}

		public static readonly DependencyProperty ParameterProperty = DependencyProperty.RegisterAttached("Address",
			typeof(string), typeof(OscAddress), new PropertyMetadata(null, PropertyChanged));

		private static void PropertyChanged(DependencyObject item, DependencyPropertyChangedEventArgs e)
		{
			item.SetValue(e.Property, e.NewValue);
		}

		public static Dictionary<DependencyObject, string> GetChildrenWithValue(DependencyObject depObj)
		{
			if (depObj is ContentControl && ((ContentControl)depObj).Content is DependencyObject)
				return GetChildrenWithValue(((ContentControl)depObj).Content as DependencyObject);

			var output = new Dictionary<DependencyObject, string>();

			if (depObj == null)
				return new Dictionary<DependencyObject, string>();

			for (int i = 0; i < VisualTreeHelper.GetChildrenCount(depObj); i++)
			{
				DependencyObject child = VisualTreeHelper.GetChild(depObj, i);
				if (child == null)
					continue;

				var para = GetAddress(child);
				if (para != null)
					output[child] = para;

				var subChildren = GetChildrenWithValue(child);
				foreach (var subChild in subChildren)
					output[subChild.Key] = subChild.Value;
			}

			return output;
		}
	}
}
