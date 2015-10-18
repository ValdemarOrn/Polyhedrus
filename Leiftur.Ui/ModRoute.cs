using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Leiftur.Ui
{
	public class ModRoute
	{
		public string Source { get; set; }
		public string Destination { get; set; }
		public string ViaSource { get; set; }
		public double Amount { get; set; }
		public double ViaAmount { get; set; }
	}
}
