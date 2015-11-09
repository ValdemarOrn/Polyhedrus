using System.Linq;
using System.Windows.Controls;

namespace Leiftur.Ui.Components
{
	/// <summary>
	/// Interaction logic for MatrixSection.xaml
	/// </summary>
	public partial class MatrixSection : UserControl
	{
		private int startIndex;

		public MatrixSection()
		{
			InitializeComponent();
		}

		public int StartIndex
		{
			get { return startIndex; }
			set { startIndex = value; SetIndex(); }
		}

		private void SetIndex()
		{
			int index = StartIndex;
			foreach (var childP in RoutesStackPanel.Children)
			{
				var childPanel = childP as StackPanel;
				if (childPanel == null)
					continue;

				var children = OscAddress.GetChildrenWithValue(childPanel);
				foreach (var child in children)
				{
					var currentVal = OscAddress.GetAddress(child.Key);
					var newVal = currentVal.Replace("*", index.ToString());
					OscAddress.SetAddress(child.Key, newVal);
				}

				index++;
			}
		}
	}
}
