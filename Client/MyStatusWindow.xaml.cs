namespace Client
{
    public partial class MyStatusWindow : System.Windows.Window
    {
        public MyStatusWindow()
        {
            InitializeComponent();

            ClientSocket.SendMessage(Constants.Client.GET_PERSONAL_STATS);
            System.Text.Json.Nodes.JsonObject j = ClientSocket.ReceiveMessage().Item2;

            gameCount.Content = "Number Of Games Played - " + j["gameCount"];
            numOfRightAnwers.Content = "Number Of Right Answers - " + j["correctAnswers"];
            numOfAnswers.Content = "Number Of Total Answers - " + j["totalAnswers"];
            avgTimePerAnswer.Content = "Average Time Per Answer - " + j["avgTimePerAnswer"];
            playerScore.Content = "Score - " + j["playerScore"];
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
