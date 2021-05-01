# Bank Account Service with Security

## Idea

- It is a bank account security system.

- Basically, the remittance provided by the bank account, balance check, etc.

- Like a security system, it's about your personal information.

- Accounts are managed encrypted.

## Basic Feature - Board

### Features

- 7 Segment - Show the account password you enter
- Dip Switch - Use as a second security device when entering an account password

- Boozer - Boozer sounds when entering wrong account passwords five times in a row
- Text LCD - Display current account balance, account number
- Touch LCD - UI Interface 구현

### Description

- Save the user's name and password as a structure when creating the first account.

- At this point, the password is provided using /dev/random, which is provided by the UNIX system itself.

- After generating with random numbers, we use RSA algorithms to generate public keys and secret keys and store them encrypted.

- You can use the account number only when the Dip Switch status matches the Binary Code.

- Check each transaction.

## Basic Feature - Transaction

- Remittance: Account password and OTP authentication required.

- Inquiry into transaction details: Request account password

- Balance check: Account password required

## Development Methods

### Encryption

- Encrypt all information using Pseudo Random Number Generation (PRNG) and RSA algorithms

### Bank Account

- Management of all personal information, account number, password, balance, and transaction details of the account holder in an encrypted state

- Manage data by leveraging tree structures

### Transaction details

- Management of details, such as types of transactions (deposits, withdrawals), transaction time, transaction amount, etc. as a structure

- Manage data by leveraging tree structures

## Scenario

![Scenario](./Scenario.png)

## Demonstration video

![Video](./AES_testing_Reduced-min.gif)
