# ---- the ingress addon is enabled in my minikube installation
minikube addons enable ingress

# ---- verify ingress infra is running
kubectl get pods -n kube-system


# ---- configure ingress nginx
kubectl apply -f https://raw.githubusercontent.com/nginxinc/kubernetes-ingress/master/deployments/common/ns-and-sa.yaml
kubectl apply -f https://raw.githubusercontent.com/nginxinc/kubernetes-ingress/master/deployments/common/default-server-secret.yaml
kubectl apply -f https://raw.githubusercontent.com/nginxinc/kubernetes-ingress/master/deployments/common/nginx-config.yaml
kubectl apply -f https://raw.githubusercontent.com/nginxinc/kubernetes-ingress/master/deployments/rbac/rbac.yaml
kubectl apply -f https://raw.githubusercontent.com/nginxinc/kubernetes-ingress/master/deployments/daemon-set/nginx-ingress.yaml

# --- check ingress nginx is running
kubectl get pods --namespace=nginx-ingress




# --- create deployment  ---> Error from server (AlreadyExists): deployments.apps "app" already exists
kubectl create deployment app --image=kcapp:1.0  
kubectl expose deployment app --type=NodePort --port=8080


# --- Verify the Service is created and is available on a node port:
kubectl get service app
#
# NAME   TYPE        CLUSTER-IP     EXTERNAL-IP   PORT(S)    AGE
# app    ClusterIP   10.99.69.170   <none>        8080/TCP   8h



test.yaml
-------------------------------------
apiVersion: extensions/v1beta1
kind: Ingress
metadata:
  name: mysample-ingress
  annotations:
    kubernetes.io/ingress.class: "nginx"
spec:
  rules:
  - host: localhost
    http:
      paths:
      - path: /
        backend:
          serviceName: mysample-service
          servicePort: 8080

