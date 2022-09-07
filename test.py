import boto3
import json
import time
import re
# from analysis import * 
session = boto3.Session()

bucket = session.resource("s3").Bucket("marathonjudgestack-marathonbuckete256be1e-1a2q8kivf5cr6")
bucket.upload_file("./main.cpp", "ahc013/main.cpp")

sfn = session.client("stepfunctions")
response = sfn.start_execution(
    input=json.dumps({"timestamp": time.time(), "contest_name": "ahc013"}),
    stateMachineArn="arn:aws:states:ap-northeast-1:905492273581:stateMachine:StateMachine2E01A3A5-Ze2uA3XD3OCz"
)

while True:
    res = sfn.describe_execution(executionArn=response["executionArn"])
    status = res["status"]
    if status == "RUNNING":
        time.sleep(5)
        continue
    else:
        break

tests = json.loads(json.loads(res["output"])["Payload"]["summary_result"]["Payload"])
sum = 0
for test in tests:
    sum += int(re.sub(r"\D", "", test['score']))
    print(test)

print('SUM : ' + str(sum))
# compare_with_base_score(scores)