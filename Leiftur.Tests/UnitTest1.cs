using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using OxyPlot;
using LowProfile.Visuals;

namespace Leiftur.Tests
{
    [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void TestMethod1()
        {
			var t = new Leiftur.Plugin.LeifturNative(48000);
			var data = t.GetWavetable(0);

			var pm = new PlotModel();
			pm.AddLine(data.WavetableData);
			pm.Show();
        }
    }
}
