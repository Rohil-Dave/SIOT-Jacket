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

# Define the UI as a selection box for the files as well as which variables to 
# plot
ui <- fluidPage(
  titlePanel("Smart Jacket Data Plotter"),
  
  sidebarLayout(
    sidebarPanel(
      fileInput("file", "Choose CSV File", accept = c(".csv")),
      checkboxGroupInput("checkGroup", "Select Sensor",choices = NULL),
      #selectInput(inputId = "selectVal", label = "Sensor type", choices = c("Left", "Right", "Ambient")),
      actionButton("plotBtn", "Plot")
    ),
    
    mainPanel(
      plotOutput("plot")
    )
  )
)

# Define server
server <- function(input, output, session) {
  # Reactive function to read CSV file and update checkbox options
  data <- reactive({
    req(input$file)
    read.csv(input$file$datapath)
  })
  
  observe({
    updateCheckboxGroupInput(session, "checkGroup", choices = c("Left", "Right", "Ambient"),
                             selected = "Ambient")
  })
  
  # Reactive function to generate plot
  output$plot <- renderPlot({
    req(input$file, input$plotBtn)
    
    if (input$checkGroup == "Left") {
      ggplot(data(), aes_string(x = 'Time.elapsed')) +
        geom_line(aes_string(y = 'Left.X.Acc'), color = 'red') +
        geom_line(aes_string(y = 'Left.Y.Acc'), color='green') +
        geom_line(aes_string(y = 'Left.Z.Acc'), color='blue') +
        geom_ribbon(aes(ymin = ifelse(Left.Sleeve.State == 1, -Inf, NA), ymax = ifelse(Left.Sleeve.State == 1, Inf, NA)),
                    fill = "lightblue", alpha = 0.5) +
        labs(title = "Left Accelerometer", x = 'Time Elapsed in ms', y = 'XYZ data (RGB)') + theme_minimal()
    } else {
      if (input$checkGroup == "Right") {
        ggplot(data(), aes_string(x = 'Time.elapsed')) +
          geom_line(aes_string(y = 'Right.X.Acc'), color = 'red') +
          geom_line(aes_string(y = 'Right.Y.Acc'), color='green') +
          geom_line(aes_string(y = 'Right.Z.Acc'), color='blue') +
          geom_ribbon(aes(ymin = ifelse(Right.Sleeve.State == 1, -Inf, NA), ymax = ifelse(Right.Sleeve.State == 1, Inf, NA)),
                      fill = "lightblue", alpha = 0.5) +
          labs(title = "Right Accelerometer", x = 'Time Elapsed in ms', y = 'XYZ data (RGB)') + theme_minimal()       
      } else {
        ggplot(data(), aes_string(x = 'Time.elapsed')) +
          geom_line(aes_string(y = 'Visible.Light'), color = 'orange') +
          geom_ribbon(aes(ymin = ifelse(Glow.State == 1, -Inf, NA), ymax = ifelse(Glow.State == 1, Inf, NA)),
                      fill = "lightblue", alpha = 0.5) +
          labs(title = "Ambient Light Sensor", x = 'Time Elapsed in ms', y = 'Luminosity (Full - IR)') + theme_minimal()
      }
    }
    
  })
}

# Run the app
shinyApp(ui, server)
