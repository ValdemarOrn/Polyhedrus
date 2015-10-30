using System.Windows;
using System.Windows.Controls;

namespace Leiftur.Ui.Components
{
	/// <summary>
	/// Interaction logic for EnvelopeSection.xaml
	/// </summary>
	public partial class EnvelopeSection : BaseControl
	{
		public static readonly DependencyProperty ShowPage2Property = DependencyProperty.Register("ShowPage2", typeof(bool), typeof(EnvelopeSection),
				new FrameworkPropertyMetadata(false, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

		public EnvelopeSection()
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
