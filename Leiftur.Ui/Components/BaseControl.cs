using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace Leiftur.Ui.Components
{
	public class BaseControl : UserControl
	{
		private string moduleAddress;

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
				var newVal = currentVal.Replace("*", moduleAddress);
				OscAddress.SetAddress(child.Key, newVal);
			}
		}
	}
}
