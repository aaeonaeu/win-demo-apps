using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409
internal enum EAPIStatus_t : uint
{
    EAPI_STATUS_SUCCESS = 0,
    EAPI_STATUS_ERROR = 0xFFFFF0FF,
    EAPI_STATUS_INITIALIZED = 0xFFFFFFFE,
    EAPI_STATUS_NOT_INITIALIZED = 0xFFFFFFFF,
};

namespace UpDemoApp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        //UpBridge
        UpBridge.Up upb = new UpBridge.Up();
        private event EventHandler UpReadydEvent;

        public MainPage()
        {
            this.InitializeComponent();
            UpReadydEvent += OnUpReady;
            pDir.Items.Add("IN");
            pDir.Items.Add("OUT");
            pVal.Items.Add("Hi");
            pVal.Items.Add("Lo");

            UpReadydEvent.Invoke(null, null);
        }
        private void OnUpReady(object sender, EventArgs e)
        {
            //Update Info
            UpMfg.Text = upb.BoardGetManufacture();
            UpBoardName.Text = "Board Name:" + upb.BoardGetName();
            UpBiosVer.Text = "BIOS Ver:" + upb.BoardGetBIOSVersion();

            //Check all pins
            EAPIStatus_t stat = (EAPIStatus_t)upb.GpioGetCaps(0, out uint gpiocnt, out uint disabled);

            if (stat == EAPIStatus_t.EAPI_STATUS_SUCCESS)
            {
                for (uint i = 0; i < gpiocnt; ++i)
                {
                    stat = (EAPIStatus_t)upb.GpioGetCaps(i, out uint cnt, out disabled);
                    if (disabled == 0x01)
                        continue;
                    stat = (EAPIStatus_t)upb.GpioGetDirection(i, 0xFFFFFFFF, out uint dir);
                    if (stat == EAPIStatus_t.EAPI_STATUS_ERROR)
                        continue;

                    stat = (EAPIStatus_t)upb.GpioGetLevel(i, 0xFFFFFFFF, out uint pLevel);
                    if (stat == EAPIStatus_t.EAPI_STATUS_ERROR)
                        continue;

                    pList.Items.Add(((uint)(i + 1)).ToString());
                }
            }
            if (pList.Items.Count > 0)
                pList.SelectedIndex = 0;
            return;
        }

        private void PList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            EAPIStatus_t stat = (EAPIStatus_t)upb.GpioGetDirection(UInt32.Parse(pList.SelectedValue.ToString()) - 1, 0xFFFFFFFF, out uint dir);
            if (dir != 0)
                pDir.SelectedIndex = 0;
            else
                pDir.SelectedIndex = 1;
            stat = (EAPIStatus_t)upb.GpioGetLevel(UInt32.Parse(pList.SelectedValue.ToString()) - 1, 0xFFFFFFFF, out uint pLevel);
            if (pLevel != 0)
                pVal.SelectedIndex = 0;
            else
                pVal.SelectedIndex = 1;

            if (pDir.SelectedIndex == 0)
                pVal.IsEnabled = false;
            else
                pVal.IsEnabled = true;
        }

        private void PDir_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (pDir.SelectedIndex == 0)   //IN
                upb.GpioSetDirection(UInt32.Parse(pList.SelectedValue.ToString()) - 1, 0xFFFFFFFF, 1);
            else
                upb.GpioSetDirection(UInt32.Parse(pList.SelectedValue.ToString()) - 1, 0xFFFFFFFF, 0);

            if (pDir.SelectedIndex == 0)
                pVal.IsEnabled = false;
            else
                pVal.IsEnabled = true;

            upb.GpioGetLevel(UInt32.Parse(pList.SelectedValue.ToString()) - 1, 0xFFFFFFFF, out uint pLevel);
            if (pLevel != 0)
                pVal.SelectedIndex = 0;
            else
                pVal.SelectedIndex = 1;

        }

        private void PVal_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (pVal.SelectedIndex == 0)//Hi
                upb.GpioSetLevel(UInt32.Parse(pList.SelectedValue.ToString()) - 1, 0xFFFFFFFF, 1);
            else
                upb.GpioSetLevel(UInt32.Parse(pList.SelectedValue.ToString()) - 1, 0xFFFFFFFF, 0);
        }
    }
}
