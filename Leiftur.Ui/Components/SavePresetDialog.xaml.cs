using System;
using System.Collections.Generic;
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
using System.Windows.Shapes;
using LowProfile.Core.Extensions;

namespace Leiftur.Ui.Components
{
	/// <summary>
	/// Interaction logic for SavePresetDialog.xaml
	/// </summary>
	public partial class SavePresetDialog : Window
	{
		public SavePresetDialog()
		{
			InitializeComponent();
			this.IsVisibleChanged += (s, x) =>
			{
				this.Center();
				MainTextBox.Focus();
			};
		}

		public bool Cancelled { get; private set; }

		public string ShowDialog(string title)
		{
			TitleLabel.Content = title;
			this.ShowDialog();
			return Cancelled ? null : MainTextBox.Text;
		}

		private void Save(object sender, RoutedEventArgs e)
		{
			Cancelled = false;
			Close();
		}

		private void Cancel(object sender, RoutedEventArgs e)
		{
			Cancelled = true;
			Close();
		}
	}
}
