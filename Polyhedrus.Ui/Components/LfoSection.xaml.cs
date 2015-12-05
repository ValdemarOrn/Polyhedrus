using System.Windows;
using System.Windows.Controls;

namespace Polyhedrus.Ui.Components
{
	/// <summary>
	/// Interaction logic for LfoSection.xaml
	/// </summary>
	public partial class LfoSection : BaseControl
	{
		public static readonly DependencyProperty ShowPage2Property = DependencyProperty.Register("ShowPage2", typeof(bool), typeof(LfoSection),
				new FrameworkPropertyMetadata(false, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		public LfoSection()
		{
			InitializeComponent();
		}

		public bool ShowPage2
		{
			get { return (bool)GetValue(ShowPage2Property); }
			set { SetValue(ShowPage2Property, value); }
		}
	}
}
