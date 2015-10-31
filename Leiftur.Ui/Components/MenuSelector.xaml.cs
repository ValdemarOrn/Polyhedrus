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
using System.Windows.Navigation;
using System.Windows.Shapes;
using LowProfile.Core.Ui;

namespace Leiftur.Ui.Components
{
	/// <summary>
	/// Interaction logic for MenuSelector.xaml
	/// </summary>
	public partial class MenuSelector : ListBox
	{
		public MenuSelector()
		{
			InitializeComponent();
			SelectOptionCommand = new DelegateCommand(x => SelectedItem = x);
		}

		public DelegateCommand SelectOptionCommand { get; private set; }

		private void ContextMenu_Opened(object sender, RoutedEventArgs e)
		{
			contextMenu.Items.Clear();
			foreach (var item in Items)
			{
				contextMenu.Items.Add(new MenuItem { Command = SelectOptionCommand, CommandParameter = item, Header = item.ToString() });
			}
		}

		private void Main_MouseUp(object sender, MouseButtonEventArgs e)
		{
			ContextMenu.IsOpen = true;
		}
	}
}
