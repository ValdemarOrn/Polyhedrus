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

namespace Leiftur.Ui
{
	/// <summary>
	/// Interaction logic for OscSection.xaml
	/// </summary>
	public partial class OscSection : UserControl
	{
		private string moduleAddress;

		public OscSection()
		{
			InitializeComponent();
		}

		public string ModuleAddress
		{
			get { return moduleAddress; }
			set { moduleAddress = value; SetModuleAddress(); }
		}

		private void SetModuleAddress()
		{
			var children = OscAddress.GetChildrenWithValue(this);
			foreach (var child in children)
			{
				var currentVal = OscAddress.GetAddress(child.Key);
				var newVal = currentVal.Replace("Osc*", moduleAddress);
				OscAddress.SetAddress(child.Key, newVal);
			}
		}
	}
}
