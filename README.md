# Artifacts of research paper "WBSLT: A Framework for White-Box Encryption Based on Substitution-Linear Transformation Ciphers", for NDSS 2026 conference

The proposed WBSLT is a novel design framework for tabulated white-box implementations of substitution-linear transformation (SLT) ciphers. This artifact presents setup, configuration, and evaluation details.

## 1. Licence

- Apache License v2.0 
- BSD 3-Clause

## 2. Description and Requirements

This section provides the necessary information for accessing the artifact, hardware and software requirements, and benchmarks.

### 2.1 How to access

The source code is available on Zenodo with DOI: https://doi.org/10.5281/zenodo.17543341. There are two alternative ways to access the artifact.

- Access through Github: https://github.com/mmt200088/NDSS2026-WBSLT.git.

- Access through Docker: https://hub.docker.com/r/gaotianchen/ndss2026-wbslt.

### 2.2 Hardware dependencies

- Hardware dependencies 1 (HD1): 

    - Raspberry Pi 4b.

    - Raspberry Pi 5.

    - SX1262 LoRa Node with Semtech SX1262 chipsets.

    - Sonoff Zigbee 3.0 USB Dongle Plus with Silicon Labs EFR32MG32 chipset.

### 2.3 Software dependencies

- Software dependencies 1 (SD1): 

    - GCC $\geq$ 8.1.

    - Python $\geq$ 3.10.

    - CMake $\geq$ 2.8.
    
- Software dependencies 2 (SD2): 

    - GCC $\geq$ 8.1.

    - Python $\geq$ 3.10.

    - CMake $\geq$ 2.8.

    - Golang $\geq$ 1.13.

    - latest pip, libgmp-dev and libntl-dev.

### 2.4 Benchmarks

- Experiments E1, E2 and E4 can be run on a commodity desktop machine, where E1 and E2 require SD1 and E4 requires SD2. Experiment E3 requires access to IoT gateways supporting communication protocols Bluetooth Low Energy (BLE), LoRaWAN and Zigbee with requirements HD1 and SD1.

- Experiment E2 involves comparison with other white-box AES (WB-AES) implementations in https://github.com/scnucrypto/WBMatrix, and we have included these implementations in the artifact.

### 2.5 List of File

The file strucure of this artifact is as follows:

```
NDSS2026-WBSLT
|-- Dockerfile
|-- requirements.txt
|-- E1-SLT-Application
|   |-- WBSLT-SLT
|   |-- run-e1.sh
|   `-- run-get-results.sh
|-- E2-AES-Performance
|   |-- CEJO-whitebox-AES-master
|   |-- Improved-Masking-for-White-box-AES-main
|   |-- Table-Redundancy-Method-for-White-box-AES-master
|   |-- Xiao-Lai-White-box-AES-master
|   |-- WBSLT-general
|   |-- WBSLT-optimized
|   |-- run-e2.sh
|   `-- run-get-results.sh
|-- E3-IoT-Integration
|   |-- ble
|   |-- lorawan
|   `-- zigbee
`-- E4-Attack-Resistance
    |-- BGE
    |-- BS
    |-- LRDRP
    |-- MGH
    |-- DCA
    |-- DFA
    |-- run-e4.sh
    `-- run-get-results.sh
```

- `Dockerfile`: The Dockerfile used to initialize the virtual machine with the right toolchain and runtime environment for the artifact.
- `requirements.txt`: List of necessary libraries and dependencies.
- `E1-SLT-Application/WBSLT-SLT/`: Directory of code to generate white-box implementations under various SLT cipher configurations.
- `E1-SLT-Application/run-e1.sh`: Script for conducting experiment 1.
- `E1-SLT-Application/run-get-results.sh`: Script for producing results and illustration figures of experiment 1.
- `E2-AES-Performance/CEJO-whitebox-AES-master`: Directory of Chow et al.'s WB-AES implementation.
- `E2-AES-Performance/Improved-Masking-for-White-box-AES-main`: Directory of Lee et al.'s WB-AES (Mask) implementation.
- `E2-AES-Performance/Table-Redundancy-Method-for-White-box-AES-master`: Directory of Lee et al.'s WB-AES (Table) implementation.
- `E2-AES-Performance/Xiao-Lai-White-box-AES-master`: Directory of Xiao et al.'s WB-AES implementation.
- `E2-AES-Performance/WBSLT-general`: Directory of our general WB-AES implementation.
- `E2-AES-Performance/WBSLT-WBSLT-optimized`: Directory of our optimized WB-AES implementation.
- `E2-AES-Performance/run-e2.sh`: Script for conducting experiment 2.
- `E2-AES-Performance/run-get-results.sh`: Script for producing results and illustration figures of experiment 2.
- `E3-IoT-Integration/ble`: Directory of code integrating our general and optimized WB-AES implementation into BLE protocol.
- `E3-IoT-Integration/lorawan`: Directory of code integrating our general and optimized WB-AES implementation into LoRaWAN protocol.
- `E3-IoT-Integration/zigbee`: Directory of code integrating our general and optimized WB-AES implementation into Zigbee protocol.
- `E4-Attack-Resistance/BGE`: Directory of BGE attack implementation.
- `E4-Attack-Resistance/BS`: Directory of BS attack implementation.
- `E4-Attack-Resistance/LRDRP`: Directory of LRDRP attack implementation.
- `E4-Attack-Resistance/MGH`: Directory of MGH attack implementation.
- `E4-Attack-Resistance/DCA`: Directory of DCA attack implementation.
- `E4-Attack-Resistance/DFA`: Directory of DFA attack implementation.
- `E4-Attack-Resistance/run-e4.sh`: Script for conducting experiment 4.
- `E4-Attack-Resistance/run-get-results.sh`: Script for producing results of experiment 4.


## 3 Artifact Installation and Configuration

This section provides the steps to install and configure the artifact using either Github or Docker.

- Access through Github:

```
$ git clone https://github.com/mmt200088/NDSS2026-WBSLT.git
$ cd NDSS2026-WBSLT
$ sh setup.sh
```
    
- Access through Docker:
```
$ docker pull gaotianchen/ndss2026-wbslt
$ docker run -it ndss2026-wbslt /bin/bash
```


## 4 Major Claims

The artifacts support the following major claims presented in our paper:

- (C1): WBSLT is a novel framework to design secure white-box implementations of arbitrary SLT ciphers with efficient encryption speed. Experiment (E1) proves this claim by: 

    - (RQ1 in initial submission) Generating white-box implementations under various SLT cipher configurations and evaluating the encryption speed, for which the results are illustrated in Figure 15.

    - (Major revision) Generating white-box implementations of WBSLT-based white-box SHARK and PRESENT for encryption speed evaluation.

- (C2) WBSLT can be applied on AES-128 and generate the general WB-AES implementation. We also present optimization strategies to decrease the storage and improve the efficiency of the general WB-AES. The generated WB-AES implementation performs reasonably well. Experiment (E2) proves this claim by:

    - (RQ2 in initial submission) Generating our optimized WB-AES implementation and compare the table generation time and encryption speed with other WB-AES implementations, for which the results are illstrated in Figure 16.

    - (Major revision) Generating our general WB-AES implementation and compare the table generation time and encryption speed with other WB-AES implementations and our optimized WB-AES.

- (C3) Our optimized and general WB-AES can be integrated into wireless communication protocols BLE, LoRaWAN and Zigbee with no efficiency degradation. Experiment (E3) proves this claim by:

    - (RQ3 in initial submission) Integrating our optimized WB-AES implementation into these protocols to compare the data transmission speed with the original AES implementation, for which the results are illustrated in Figure 17.

    - (Major revision) Integrating our optimized WB-AES implementation into these protocols to compare the data transmission speed with the original AES implementation.

- (C4): WBSLT fuzzes the boundaries of components to defend against various white-box attacks. Experiment (E4) proves this claim by:

    - (Initial submission) Conducting DCA and DFA attacks on WBSLT, for which the analysis and results are illustrated in Section 4.3.

    - (Major revision) Conducting algebraic and structural attacks (i.e., BGE, MGH, LRDRP and BS attacks) on WBSLT, for which the analysis are illustrated in Section 4.2.


## 5 Evaluation

The following experiments prove the major claims above and validate the functionality of the artifacts.

### 5.1 Experiment (E1) [SLT Application Experiment] [5 human-minutes + 6 compute-hour]

This experiment generates white-box implementations under various SLT configurations, and evaluates their encryption speed using the generated implementations.

#### 5.1.1 Execution

Execute the following command to run automatic script for experiment data collection:

```
$ cd NDSS2026-WBSLT/E1-SLT-Application
$ sh run-e1.sh
```

After completion, the results will be stored in `NDSS2026-WBSLT/E1-SLT-Application/results/`.

#### 5.1.2 Results

Execute the following command to parse the experiment data for results and illustration figure:

```
$ sh run-get-results.sh
```

After completion, the data will be stored in `NDSS2026-WBSLT/E1-SLT-Application/results/data.csv`, and an illustration figure will be stored in `NDSS2026-WBSLT/E1-SLT-Application/results/figure.png`. The results will follow similar trends in Figure 15.

### 5.2 Experiment (E2) [AES Performance Experiment] [5 human-minutes + 4 compute-hour]

This experiment generates our general and optimized white-box implementations, and evaluates the table generation and encryption efficiency.

#### 5.2.1 Execution

Execute the following command to run automatic script for experiment data collection:

```
$ cd NDSS2026-WBSLT/E2-AES-Performance
$ sh run-e2.sh
```

After completion, the results will be stored in `NDSS2026-WBSLT/E2-AES-Performance/results/`.

#### 5.2.2 Results

Execute the following command to parse the experiment data for results and illustration figure:

```
$ sh run-get-results.sh
```

After completion, the data will be stored in `NDSS2026-WBSLT/E2-AES-Performance/results/data.csv`, and an illustration figure will be stored in `NDSS2026-WBSLT/E2-AES-Performance/results/figure.png`. The results will follow similar trends in Figure 16.

### 5.3 Experiment (E3) [IoT Integration Experiment] [20 human-minutes + 1 compute-hour]

This experiment tests IoT protocol communication efficiency using traditional AES implementation and our general and optimized WB-AES implementations.

#### 5.3.1 Preparation

Follow these steps to configure the VPN using MotionPro Client:

1. **Download and Install MotionPro Client:**
   - Visit the [official website](https://support.arraynetworks.net/prx/000/http/supportportal.arraynetworks.net/downloads/downloads.html): https://support.arraynetworks.net/prx/000/http/supportportal.arraynetworks.net/downloads/downloads.html
   - Download the client and follow the installation instructions.

2. **Launch MotionPro Client:**
   - Once installed, open the MotionPro Client.

3. **Configure the VPN Settings:**
   - Click on **Add/Add Profile** to begin the setup.

4. **Fill in the Configuration Fields:**
   - **Title/Site Name**: Enter any name you prefer.
   - **Gateway/Host**: Enter the provided gateway.
   - **Username**: Enter the provided username.
   - **Port**: Enter the provided port.
   - Check the **Save password** box.
   - **Password**: After checking the box, the password field will become active. Enter the provided password.

5. **Save Your Configuration:**
   - Once you've filled in the required fields, click **OK/Save**.

6. **Connect to the VPN:**
   - You should now see a VPN record with the name you chose.
   - Double-click the record to establish the VPN connection.

#### 5.3.2 Execution

**Please finish the `5.3.1 Preparation` section before conducting the following experiments.**

There are a client node and a server node in this experiment.

Connect to the client and server node with provided information.

Once connected to both devices, execute the following command on both:

```
cd ~/e3-iot-protocol
```

**Bluetooth Low Energy (BLE) evaluation**

First, on the server execute:

```
./ble.sh
```

Then on the client execute:

```
./ble.sh
```

**LoRaWAN evaluation**

First, on the server execute:

```
./lorawan.sh
```

Then on the client execute:

```
./lorawan.sh
```

**Zigbee evaluation**

First, on the server execute:

```
./zigbee.sh
```

Then on the client execute:

```
./zigbee.sh
```

#### 5.3.3 Results

After each protocol evaluation, the result will be displayed on the terminal, they should indicate no data transmission speed degradation.

### 5.4 Experiment (E4) [Attack Resistance Experiment] [5 human-minutes + 7 compute-hour]

This experiment conducts various white-box attacks on our WBSLT to test the resistance.

#### 5.4.1 Execution

Execute the following command to run automatic script for experiment data collection:

```
$ cd NDSS2026-WBSLT/E4-Attack-Resistance
$ sh run-e4.sh
```

After completion, the results will be stored in `NDSS2026-WBSLT/E4-Attack-Resistance/results/`.

#### 5.4.2 Results

Execute the following command to parse the experiment data for results:

```
$ sh run-get-results.sh
```

After completion, the result will be displayed on the terminal. The attack results of DCA attack should follow similar trends in Table 4, and the results of other attacks should have zero success time on WBSLT.
