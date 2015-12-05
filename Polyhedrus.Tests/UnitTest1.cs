using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using OxyPlot;
using LowProfile.Visuals;
using System.IO;
using System.Linq;
using LowProfile.Core.Extensions;
using OxyPlot.Series;
using Polyhedrus.Plugin;
using System.Collections.Generic;

namespace Polyhedrus.Tests
{
    [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void TestMethod1()
        {
			var t = new PolyhedrusNative(48000, 12003, 12004);
			var data = t.GetWavetable(0);

			var pm = new PlotModel();
			pm.AddLine(data.WavetableData.Take(20000));
			pm.Show();
        }

		[TestMethod]
		public void RunCppTests()
		{
			PolyhedrusNative.RunTests();
			Assert.IsTrue(true);
		}

		[TestMethod]
		public void TestMethodSlop()
		{
			var data = new double[48000];
			var dataF = new double[48000];
			var random = new Random(0);

			double sum = 0.0;
			double filter = 0.0;
			var fs = 4800;
			Func<double, double> fcToAlpha = fc => (2 * Math.PI * fc / fs) / (2 * Math.PI * fc / fs + 1);
			var freq = 1.0;
			var samplesPerPoint = (int)(fs / (freq * 10));

			var aHp = fcToAlpha(freq * 0.5);
			var aLp = fcToAlpha(freq * 0.2);

			var scale = 1.0 / samplesPerPoint;
			var sample = 0.0;
			var n = 0;
			for (int i = 0; i < data.Length; i++)
			{
				if (n <= 0)
				{
					sample = (2 * random.NextDouble() - 1) * scale;
					n = samplesPerPoint;
				}

				sum += sample;
				filter = filter * (1 - aLp) + aLp * sum;
				data[i] = sum;
				dataF[i] = filter;
				sum *= (1 - aHp);

				n--;
			}

			var pm = new PlotModel();
			pm.AddLine(data);
			pm.AddLine(dataF);
			pm.Show();
		}
	}
}
