namespace Client
{
    public partial class MenuWindow : System.Windows.Window
    {
        public MenuWindow()
        {
            this.InitializeComponent();
            UserLabel.Content = "Hello '" + ClientHelper.current_user + '\'';
        }

        private void Click_Signout(object sender, System.Windows.RoutedEventArgs e)
        {
            ClientSocket.SendMessage(Constants.Client.LOGOUT);
            ClientSocket.ReceiveMessage(); // Dummy.

            ClientHelper.SwitchToParentWindow(this);
        }
        private void Click_Join(object sender, System.Windows.RoutedEventArgs e)
        {
            ClientHelper.SwitchToNewWindow(this, new Client.JoinRoomWindow());
        }
        private void Click_CreateRoom(object sender, System.Windows.RoutedEventArgs e)
        {
            ClientHelper.SwitchToNewWindow(this, new Client.CreateRoomWindow());
        }
        private void Click_MyStatus(object sender, System.Windows.RoutedEventArgs e)
        {
            ClientHelper.SwitchToNewWindow(this, new Client.MyStatusWindow());
        }
        private void Click_BestScores(object sender, System.Windows.RoutedEventArgs e)
        {
            ClientHelper.SwitchToNewWindow(this, new Client.BestScoresWindow());
        }

        protected override void OnClosed(System.EventArgs e)
        {
            if (ClientHelper.doClose)
            {
                ClientSocket.Destructor();
                base.OnClosed(e);
                System.Environment.Exit(0);
            }
            ClientHelper.doClose = true;
        }
    }
}
