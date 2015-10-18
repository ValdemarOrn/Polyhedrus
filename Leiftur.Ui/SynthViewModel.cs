using LowProfile.Core.Ui;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Leiftur.Ui
{
	public class SynthViewModel : ViewModelBase
	{
		private ModRoute[] modRoutes;

		public SynthViewModel()
		{
			ModRoutes = Enumerable.Range(0, 12).Select(x => new ModRoute()).ToArray();
		}

		public ModRoute[] ModRoutes
		{
			get { return modRoutes; }
			set { modRoutes = value; }
		}
	}
}
