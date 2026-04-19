namespace Client
{
    public partial class SignupWindow : System.Windows.Window
    {
        public SignupWindow()
        {
            this.InitializeComponent();
        }

        private void Click_Signup(object sender, System.Windows.RoutedEventArgs e)
        {
            string email = SignupEmailBox.Text;
            string username = SignupUsernameBox.Text;
            string password = SignupPasswordBox.Text;

            SignupEmailBox.Text = "";
            SignupUsernameBox.Text = "";
            SignupPasswordBox.Text = "";

            if (email == "" || username == "" || password == "")
            {
                ClientHelper.Popup("Please don't leave empty boxes");
                return;
            }

            System.Text.Json.Nodes.JsonObject j = new System.Text.Json.Nodes.JsonObject();
            j["email"] = email;
            j["username"] = username;
            j["password"] = password;

            ClientSocket.SendMessage(Constants.Client.SIGNUP, j);
            Tuple<byte, System.Text.Json.Nodes.JsonObject> response = ClientSocket.ReceiveMessage();

            if (response.Item1 == Constants.Server.SIGNUP)
            {
                ClientHelper.SwitchToParentWindow(this);
            }
        }

        private void Click_GoBack(object sender, System.Windows.RoutedEventArgs e)
        {
            ClientHelper.SwitchToParentWindow(this);
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
