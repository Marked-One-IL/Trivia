namespace Client
{
    public partial class BestScoresWindow : System.Windows.Window
    {
        public BestScoresWindow()
        {
            this.InitializeComponent();

            ClientSocket.SendMessage(Constants.Client.GET_HIGH_SCORE);
            Tuple<byte, System.Text.Json.Nodes.JsonObject> response = ClientSocket.ReceiveMessage();
            System.Text.Json.Nodes.JsonObject j = response.Item2;

            if (j.ContainsKey("names")) // Could happen if there is no enough data in the DB.
            {
                switch (j["names"].AsArray().Count)
                {
                    case 5:
                        Player0Label.Content = j["names"][0].ToString() + " - " + j["scores"][0].ToString();
                        Player1Label.Content = j["names"][1].ToString() + " - " + j["scores"][1].ToString();
                        Player2Label.Content = j["names"][2].ToString() + " - " + j["scores"][2].ToString();
                        Player3Label.Content = j["names"][3].ToString() + " - " + j["scores"][3].ToString();
                        Player4Label.Content = j["names"][4].ToString() + " - " + j["scores"][4].ToString();
                        break;

                    case 4:
                        Player0Label.Content = j["names"][0].ToString() + " - " + j["scores"][0].ToString();
                        Player1Label.Content = j["names"][1].ToString() + " - " + j["scores"][1].ToString();
                        Player2Label.Content = j["names"][2].ToString() + " - " + j["scores"][2].ToString();
                        Player3Label.Content = j["names"][3].ToString() + " - " + j["scores"][3].ToString();
                        break;

                    case 3:
                        Player0Label.Content = j["names"][0].ToString() + " - " + j["scores"][0].ToString();
                        Player1Label.Content = j["names"][1].ToString() + " - " + j["scores"][1].ToString();
                        Player2Label.Content = j["names"][2].ToString() + " - " + j["scores"][2].ToString();
                        break;

                    case 2:
                        Player0Label.Content = j["names"][0].ToString() + " - " + j["scores"][0].ToString();
                        Player1Label.Content = j["names"][1].ToString() + " - " + j["scores"][1].ToString();
                        break;

                    case 1:
                        Player0Label.Content = j["names"][0].ToString() + " - " + j["scores"][0].ToString();
                        break;

                }
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
