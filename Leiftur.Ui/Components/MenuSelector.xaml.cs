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
			var me = new MenuItem();

			var items = new List<KeyValuePair<int, string>>();
			foreach (var item in Items)
				items.Add((KeyValuePair<int, string>)item);

			AddMenuItems(contextMenu, items.Select(x => Tuple.Create(x, x.Value.Split('/'))).ToArray());
			
		}

		/// <summary>
		/// Splits slash-separated strings into a submenu tree
		/// </summary>
		/// <param name="menu"></param>
		/// <param name="items"></param>
		private void AddMenuItems(ItemsControl menu, Tuple<KeyValuePair<int, string>, string[]>[] items)
		{
			foreach (var group in items.Where(x => x.Item2.Length > 1).GroupBy(x => x.Item2.First()))
			{
				var menuItem = new MenuItem { Command = null, CommandParameter = null, Header = group.Key };
				menu.Items.Add(menuItem);
				var subItems = group.Select(x => Tuple.Create(x.Item1, x.Item2.Skip(1).ToArray())).ToArray();
				AddMenuItems(menuItem, subItems);
			}

			foreach (var item in items.Where(x => x.Item2.Length == 1))
			{
				menu.Items.Add(new MenuItem { Command = SelectOptionCommand, CommandParameter = item.Item1, Header = item.Item2.Last() });
			}
		}

		private void Main_MouseUp(object sender, MouseButtonEventArgs e)
		{
			ContextMenu.IsOpen = true;
		}
	}
}
