# This is a simple visualization app that plots the data collected by the Smart
# Jacket system. The data is in the form of a csv file that has 10 columns.
# The columns start with 'Time.elapsed' in ms and includes visibility data, left
# and right hand accelerometer data, and the LED on/off state for the left/right
# sleeve + glow state
#
# Author: Rohil J Dave
#

# Load required libraries
library(shiny)
library(ggplot2)

# Define the UI
ui <- fluidPage(
  titlePanel("Smart Jacket Data Plotter"),
  
  sidebarLayout(
    sidebarPanel(
      actionButton("ride1", "Ride 1"), # Buttons to select which ride to analyze
      actionButton("ride2", "Ride 2"),
      actionButton("ride3", "Ride 3"),
      actionButton("ride4", "Ride 4"),
      actionButton("ride5", "Ride 5")
    ),
    
    mainPanel(
      plotOutput("ambientPlot"),
      plotOutput("leftPlot"),
      plotOutput("rightPlot")
    )
  )
)

# Define server
server <- function(input, output) {
  
  # Function to read CSV and generate plots
  renderRideData <- function(file) {
    data <- read.csv(file)
    
    # Convert Time.elapsed from milliseconds to minutes
    data$Time.elapsed <- data$Time.elapsed / 60000  # Convert to minutes
    
    list(
      # Creates graph for left acc
      leftPlot = ggplot(data, aes(x = Time.elapsed)) +
        geom_line(aes(y = Left.X.Acc, color = "X")) +
        geom_line(aes(y = Left.Y.Acc, color = "Y")) +
        geom_line(aes(y = Left.Z.Acc, color = "Z")) +
        geom_ribbon(aes(ymin = ifelse(Left.Sleeve.State == 1, -Inf, NA), ymax = ifelse(Left.Sleeve.State == 1, Inf, NA), fill = "Right Sleeve on"),
                    alpha = 0.5) +
        labs(title = "Left Accelerometer", x = 'Time Elapsed (minutes)', y = 'Acceleration (m/s^2)') + theme_minimal() +
        scale_fill_manual(values = c("Right Sleeve on" = "orange")) + 
        scale_color_manual(values = c("X" = "red", "Y" = "green", "Z" = "blue")),
      
      # Creates graph for right acc
      rightPlot = ggplot(data, aes(x = Time.elapsed)) +
        geom_line(aes(y = Right.X.Acc, color = "X")) +
        geom_line(aes(y = Right.Y.Acc, color = "Y")) +
        geom_line(aes(y = Right.Z.Acc, color = "Z")) +
        geom_ribbon(aes(ymin = ifelse(Right.Sleeve.State == 1, -Inf, NA), ymax = ifelse(Right.Sleeve.State == 1, Inf, NA), fill = "Left Sleeve on"),
                    alpha = 0.5) +
        labs(title = "Right Accelerometer", x = 'Time Elapsed (minutes)', y = 'Acceleration (m/s^2)') + theme_minimal() +
        scale_fill_manual(values = c("Left Sleeve on" = "orange")) + 
        scale_color_manual(values = c("X" = "red", "Y" = "green", "Z" = "blue")),
      
      # Creates graph for light readings
      ambientPlot = ggplot(data, aes(x = Time.elapsed)) +
        geom_line(aes(y = Visible.Light, color = "visible light")) +
        geom_ribbon(aes(ymin = ifelse(Glow.State == 1, -Inf, NA), ymax = ifelse(Glow.State == 1, Inf, NA), fill = "Glow on"),
                    alpha = 0.5) +
        labs(title = "Ambient Light Sensor", x = 'Time Elapsed (minutes)', y = 'Visible Light reading') + theme_minimal() +
        scale_fill_manual(values = c("Glow on" = "orange")) + 
        scale_color_manual(values = c("visible light" = "hotpink"))
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
  
  
  # Render Plots
  output$leftPlot <- renderPlot({ values$leftPlot })
  output$rightPlot <- renderPlot({ values$rightPlot })
  output$ambientPlot <- renderPlot({ values$ambientPlot })
}

# Run the app
shinyApp(ui, server)