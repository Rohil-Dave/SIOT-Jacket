# Load required libraries
library(shiny)
library(ggplot2)
library(plotly)

# Define the UI
ui <- fluidPage(
  titlePanel("Smart Jacket Data Plotter"),
  
  sidebarLayout(
    sidebarPanel(
      actionButton("ride1", "Ride 1"),
      actionButton("ride2", "Ride 2"),
      actionButton("ride3", "Ride 3"),
      actionButton("ride4", "Ride 4"),
      actionButton("ride5", "Ride 5")
    ),
    
    mainPanel(
      plotlyOutput("leftPlot"),
      plotlyOutput("rightPlot"),
      plotlyOutput("ambientPlot")
    )
  )
)

# Define server
server <- function(input, output) {
  
  # Function to read CSV and generate interactive plots
  renderRideData <- function(file) {
    data <- read.csv(file)
    
    # Convert Time.elapsed from milliseconds to minutes
    data$Time.elapsed <- data$Time.elapsed / 60000  # Convert to minutes
    
    list(
      leftPlot = ggplotly(
        ggplot(data, aes(x = Time.elapsed)) +
          geom_line(aes(y = Left.X.Acc, color = "X-Axis")) +
          geom_line(aes(y = Left.Y.Acc, color = "Y-Axis")) +
          geom_line(aes(y = Left.Z.Acc, color = "Z-Axis")) +
          geom_ribbon(aes(ymin = ifelse(Left.Sleeve.State == 1, -Inf, NA), ymax = ifelse(Left.Sleeve.State == 1, Inf, NA), fill = "Sleeve State"),
                      alpha = 0.5) +
          labs(title = "Left Accelerometer", x = 'Time Elapsed (minutes)', y = 'Acceleration') + theme_minimal() +
          scale_fill_manual(values = c("Sleeve State" = "orange")) +
          scale_color_manual(values = c("X-Axis" = "red", "Y-Axis" = "green", "Z-Axis" = "blue"))
      ),
      rightPlot = ggplotly(
        ggplot(data, aes(x = Time.elapsed)) +
          geom_line(aes(y = Right.X.Acc, color = "X-Axis")) +
          geom_line(aes(y = Right.Y.Acc, color = "Y-Axis")) +
          geom_line(aes(y = Right.Z.Acc, color = "Z-Axis")) +
          geom_ribbon(aes(ymin = ifelse(Right.Sleeve.State == 1, -Inf, NA), ymax = ifelse(Right.Sleeve.State == 1, Inf, NA), fill = "Sleeve State"),
                      alpha = 0.5) +
          labs(title = "Right Accelerometer", x = 'Time Elapsed (minutes)', y = 'Acceleration') + theme_minimal() +
          scale_fill_manual(values = c("Sleeve State" = "orange")) +
          scale_color_manual(values = c("X-Axis" = "red", "Y-Axis" = "green", "Z-Axis" = "blue"))
      ),
      ambientPlot = ggplotly(
        ggplot(data, aes(x = Time.elapsed)) +
          geom_line(aes(y = Visible.Light, color = "visible light reading")) +
          geom_ribbon(aes(ymin = ifelse(Glow.State == 1, -Inf, NA), ymax = ifelse(Glow.State == 1, Inf, NA), fill = "Glow State"),
                      alpha = 0.5) +
          labs(title = "Ambient Light Sensor", x = 'Time Elapsed (minutes)', y = 'Visible Light Reading') + theme_minimal() +
          scale_fill_manual(values = c("Glow State" = "orange")) +
          scale_color_manual(values = c("visible light reading" = "hotpink"))
      )
    )
  }
  
  # Reactive values for the plots
  values <- reactiveValues(leftPlot = NULL, rightPlot = NULL, ambientPlot = NULL)
  
  # Observers for each ride button
  observeEvent(input$ride1, {
    plots <- renderRideData("ride_data_Daytime1.csv")
    values$leftPlot <- plots$leftPlot
    values$rightPlot <- plots$rightPlot
    values$ambientPlot <- plots$ambientPlot
  })
  observeEvent(input$ride2, {
    plots <- renderRideData("ride_data_Daytime2.csv")
    values$leftPlot <- plots$leftPlot
    values$rightPlot <- plots$rightPlot
    values$ambientPlot <- plots$ambientPlot
  })
  observeEvent(input$ride3, {
    plots <- renderRideData("ride_data_Daytime3.csv")
    values$leftPlot <- plots$leftPlot
    values$rightPlot <- plots$rightPlot
    values$ambientPlot <- plots$ambientPlot
  })
  observeEvent(input$ride4, {
    plots <- renderRideData("ride_data_Nighttime1.csv")
    values$leftPlot <- plots$leftPlot
    values$rightPlot <- plots$rightPlot
    values$ambientPlot <- plots$ambientPlot
  })
  observeEvent(input$ride5, {
    plots <- renderRideData("ride_data_Nighttime2.csv")
    values$leftPlot <- plots$leftPlot
    values$rightPlot <- plots$rightPlot
    values$ambientPlot <- plots$ambientPlot
  })
  
  # Render Interactive Plots
  output$leftPlot <- renderPlotly({ values$leftPlot })
  output$rightPlot <- renderPlotly({ values$rightPlot })
  output$ambientPlot <- renderPlotly({ values$ambientPlot })
}

# Run the app
shinyApp(ui, server)
