namespace Client
{
    public partial class CreateRoomWindow : System.Windows.Window
    {
        public CreateRoomWindow()
        {
            this.InitializeComponent();
        }

        private void Click_CreateRoom(object sender, System.Windows.RoutedEventArgs e)
        {
            if (playerCount.Text == "" || timePerQuestion.Text == "" || QuestionCount.Text == "" || RoomName.Text == "")
            {
                ClientHelper.Popup("Please don't leave empty boxes");
                return;
            }
            else if(int.Parse(QuestionCount.Text) > 10)
            {
                ClientHelper.Popup("Question Count Is Limited To 10 Questions");
                return;
            }
            try
            {
                if (int.Parse(timePerQuestion.Text) <= 0 || int.Parse(QuestionCount.Text) <= 0 || int.Parse(playerCount.Text) <= 0)
                {
                    ClientHelper.Popup("Please enter valid values");
                    return;
                }
            }
            catch
            {
                ClientHelper.Popup("Please enter valid values");
                return;
            }

            System.Text.Json.Nodes.JsonObject room = new System.Text.Json.Nodes.JsonObject();

            try
            {
                room["maxUsers"] = int.Parse(playerCount.Text);
                room["answerTimeout"] = int.Parse(timePerQuestion.Text);
                room["questionCount"] = int.Parse(QuestionCount.Text);
            }
            catch
            {
                ClientHelper.Popup("Please enter valid numbers");
                return;
            }

            room["roomName"] = RoomName.Text;
            ClientSocket.SendMessage(Constants.Client.CREATE_ROOM, room);
            byte code = ClientSocket.ReceiveMessage().Item1;

            if (code != Constants.Server.FAILURE)
            {
                ClientHelper.SwitchToNewWindow(this, new RoomWindow(RoomName.Text));
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
