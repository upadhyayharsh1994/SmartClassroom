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
        ProjectionExpression:"student_name",
        ExpressionAttributeNames:{
            "#student_name":"student_name",
            "#checkin_date":"checkin_date"
        },
        ExpressionAttributeValues: {
        ":stud_name":String(event.name),
        ":c_date":String(onlyDate)
        },
        FilterExpression: "#student_name = :stud_name AND #checkin_date = :c_date"
    };
    
   
        var params2 = {
        TableName:'StudentData',
        Item:{
            "DateIn":{
                S: String(event.key_value)
            },
            "checkin_date":{
                S: String(onlyDate)
            },
            "student_name":{
                S: String(event.name)
            },
            "in_time":{
                S:String(dates.split(" ")[4])
            }
        }
    };
    
        try{
            var x = await doc.scan(params).promise();
            var response = x;
            console.log(x);
            if(x.Count === 0)
            {
                try
                {
                var res = await ddb.putItem(params2).promise();
                }
                catch(errs)
                {
                    console.log(errs);
                }
            }
            else
            {
                console.log("Not allowed!");
            }
        }
        catch(er)
        {
             var response = er;
             console.log(er);
        }
        return(true);
}
