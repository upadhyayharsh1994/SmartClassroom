 var AWS = require('aws-sdk');
  var moment = require('moment');
 var momentTz = require('moment-timezone');
exports.handler = async (event) => {
    // TODO implement
    AWS.config.update({region: 'us-west-2'});
    var ddb = new AWS.DynamoDB({apiVersion: '2012-08-10'});
    var doc  = new AWS.DynamoDB.DocumentClient();
    //var response = "";
    var k = event.temp;
    
    
    var date = new Date().toISOString();
    var date = moment(date);
    
    date.tz('America/Los_Angeles');
    var dates = String(date);
    var onlyDate = dates.split(" ")[1].concat(" ").concat(dates.split(" ")[2]).concat(" ").concat(dates.split(" ")[3])
    
        var params = {
        TableName:"StudentData",
        ProjectionExpression:"out_time,in_time",
        ExpressionAttributeNames:{
            "#student_name":"DateIn",
        },
        ExpressionAttributeValues: {
        ":stud_name":String(event.key_value),
        },
        FilterExpression: "#student_name = :stud_name"
    };

            try{
                 var y = await doc.scan(params).promise();
                 var response = y;
                if(y.Items[0].out_time === undefined && y.Items[0].in_time !== undefined)
                    {
            
                        try{
                                var timeStart = new Date("01/01/2007 " + y.Items[0].in_time);
                                var timeEnd = new Date("01/01/2007 " + String(dates.split(" ")[4]));

                                var difference = timeEnd - timeStart;             

                                difference = difference / 60 / 60 / 1000;
                                
                                var params2 = {
                                TableName:"StudentData",
                                Key:{
                                "DateIn":String(event.key_value)
                                    },
                                UpdateExpression: "SET out_time = :out_time, hours = :hours",
                                ExpressionAttributeValues:{
                                ":out_time": String(dates.split(" ")[4]),
                                ":hours": String(difference)
                                    },
                                ReturnValues:"UPDATED_NEW"
                                    };
                            var x = await doc.update(params2).promise();
                            var response = x;
                            console.log(x);
                        }
                        catch(er)
                        {
                            var response = er;
                            console.log(er);
                        }
                    }
            }
        catch(e)
        {
            
        }

            
            return x;
};
