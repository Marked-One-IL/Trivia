namespace Client
{
    public partial class MainWindow : System.Windows.Window
    {
        public MainWindow()
        {
            while (!ClientSocket.Initialize())
            {
                ClientSocket.Destructor();

                System.Windows.MessageBoxResult result = System.Windows.MessageBox.Show(
                "Cannot connect to server. Would you like to retry?",
                "Connection Error",
                System.Windows.MessageBoxButton.YesNo,
                System.Windows.MessageBoxImage.Error);

                if (result == System.Windows.MessageBoxResult.No)
                {
                    System.Environment.Exit(0);
                }
            }

            this.InitializeComponent();
        }

        private void Click_Signup(object sender, System.Windows.RoutedEventArgs e)
        {
            ClientHelper.SwitchToNewWindow(this, new Client.SignupWindow());
        }
        private void Click_Login(object sender, System.Windows.RoutedEventArgs e)
        {
            string username = UsernameBox.Text;
            string password = PasswordBox.Text;

            if (username == "" || password == "")
            {
                ClientHelper.Popup("Please don't leave empty boxes");
                return;
            }

            UsernameBox.Text = "";
            PasswordBox.Text = "";

            System.Text.Json.Nodes.JsonObject j = new System.Text.Json.Nodes.JsonObject();
            j["username"] = username;
            j["password"] = password;

            ClientSocket.SendMessage(Constants.Client.LOGIN, j);
            Tuple<byte, System.Text.Json.Nodes.JsonObject> response = ClientSocket.ReceiveMessage();

            if (response.Item1 == Constants.Server.LOGIN)
            {
                ClientHelper.current_user = username;
                ClientHelper.SwitchToNewWindow(this, new Client.MenuWindow());
            }
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