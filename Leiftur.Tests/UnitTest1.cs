using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using OxyPlot;
using LowProfile.Visuals;
using System.IO;
using System.Linq;
using LowProfile.Core.Extensions;
using OxyPlot.Series;
using Leiftur.Plugin;

namespace Leiftur.Tests
{
    [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void TestMethod1()
        {
			var t = new LeifturNative(48000);
			var data = t.GetWavetable(0);

			var pm = new PlotModel();
			pm.AddLine(data.WavetableData);
			pm.Show();
        }

		[TestMethod]
		public void RunCppTests()
		{
			LeifturNative.RunTests();
		}
    }
}
