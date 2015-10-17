using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using OxyPlot;
using LowProfile.Visuals;
using System.IO;
using System.Linq;
using LowProfile.Core.Extensions;
using OxyPlot.Series;
using Leiftur.Plugin;
using System.Collections.Generic;

namespace Leiftur.Tests
{
    [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void TestMethod1()
        {
			var t = new LeifturNative(48000, 12003, 12004);
			var data = t.GetWavetable(0);

			var pm = new PlotModel();
			pm.AddLine(data.WavetableData);
			pm.Show();
        }

		[TestMethod]
		public void RunCppTests()
		{
			LeifturNative.RunTests();
			Assert.IsTrue(true);
		}

		[TestMethod]
		public void TestMethodSlop()
		{
			var data = new double[4800];
			var dataF = new double[4800];

			double sum = 0.0;
			var fs = 4800;
			Func<double, double> fcToAlpha = fc => (2 * Math.PI * fc / fs) / (2 * Math.PI * fc / fs + 1);
			var freq = 1.0;
			var scaler = freq < 1.0 ? 1.0 / freq : 1.0;
			var samples = (int)(fs / freq);
			var incs = new double[6];
			var weights = new double[] { 1.0, 0.5, 0.25, 0.125, 0.625, 0.03125 };
			var random = new Random();
			var filter = 0.0;

			var aHp = fcToAlpha(0.1);
			var aLp = fcToAlpha(0.2);

			for (int i = 0; i < data.Length; i++)
			{
				for (int n = 0; n < incs.Length; n++)
				{
                    var limit = (int)(samples * weights[n]);

					if (i % limit == 0)
						incs[n] = (2 * random.NextDouble() - 1);

					sum += incs[n] * weights[n];
				}

				filter = filter * (1 - aLp) + aLp * sum;
				data[i] = sum * scaler;
				dataF[i] = filter * scaler;
				sum *= (1 - aHp);
			}

			var pm = new PlotModel();
			pm.AddLine(data);
			pm.AddLine(dataF);
			pm.Show();
		}
	}
}
